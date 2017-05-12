//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "BsBuiltinResourcesHelper.h"
#include "BsFileSystem.h"
#include "BsImporter.h"
#include "BsResources.h"
#include "BsShaderImportOptions.h"
#include "BsTextureImportOptions.h"
#include "BsRendererMaterialManager.h"
#include "BsRendererMaterial.h"
#include "BsFontImportOptions.h"
#include "BsSpriteTexture.h"
#include "BsTexture.h"
#include "BsRTTIType.h"
#include "BsDataStream.h"
#include "BsResourceManifest.h"
#include "BsFileSystem.h"
#include "BsCoreThread.h"
#include "BsUUID.h"
#include "BsShader.h"
#include "BsPass.h"
#include "BsGpuProgram.h"

using json = nlohmann::json;

namespace bs
{
	bool BuiltinResourcesHelper::importAssets(const nlohmann::json& entries, const Path& inputFolder, 
		const Path& outputFolder, const SPtr<ResourceManifest>& manifest, AssetType mode)
	{
		if (!FileSystem::exists(inputFolder))
			return true;

		if (FileSystem::exists(outputFolder))
			FileSystem::remove(outputFolder);
		
		FileSystem::createDir(outputFolder);

		Path spriteOutputFolder = outputFolder + "/Sprites/";
		if(mode == AssetType::Sprite)
			FileSystem::createDir(spriteOutputFolder);

		auto importResource = [&](const String& fileName, const String& UUID)
		{
			Path filePath = inputFolder + fileName;
			Vector<std::pair<Path, SPtr<ImportOptions>>> resourcesToSave;

			{
				Path relativePath = fileName;
				Path relativeAssetPath = fileName;
				relativeAssetPath.setFilename(relativeAssetPath.getWFilename() + L".asset");

				SPtr<ImportOptions> importOptions = gImporter().createImportOptions(filePath);
				if (importOptions != nullptr)
				{
					if (rtti_is_of_type<TextureImportOptions>(importOptions))
					{
						SPtr<TextureImportOptions> texImportOptions = std::static_pointer_cast<TextureImportOptions>(importOptions);
						texImportOptions->setGenerateMipmaps(false);

						resourcesToSave.push_back(std::make_pair(relativeAssetPath, texImportOptions));
					}
					else if (rtti_is_of_type<ShaderImportOptions>(importOptions))
					{
						// Check if the shader is used for a renderer material, in which case generate different variations
						// according to #defines (if any are specified).
						Vector<ShaderDefines> variations = RendererMaterialManager::_getVariations(relativePath);

						if (variations.size() == 0) // Not a renderer material or no variations, save normally
						{
							resourcesToSave.push_back(std::make_pair(relativeAssetPath, nullptr));
						}
						else // Renderer material, save a copy for each variation
						{
							// Note: Renderer materials are returned in an undefined order, meaning that renderer materials
							// will not properly persist references. But this should be okay since they are used only in low
							// level systems.

							UINT32 variationIdx = 0;
							for (auto& variation : variations)
							{
								SPtr<ShaderImportOptions> shaderImportOptions =
									std::static_pointer_cast<ShaderImportOptions>(gImporter().createImportOptions(filePath));

								shaderImportOptions->getDefines() = variation.getAll();

								Path uniquePath = RendererMaterialManager::_getVariationPath(relativePath, variationIdx);
								uniquePath.setFilename(uniquePath.getWFilename() + L".asset");

								resourcesToSave.push_back(std::make_pair(uniquePath, shaderImportOptions));
								variationIdx++;
							}
						}
					}
					else
						resourcesToSave.push_back(std::make_pair(relativeAssetPath, nullptr));
				}
				else
					resourcesToSave.push_back(std::make_pair(relativeAssetPath, nullptr));
			}

			// Use the provided UUID if just one resource, otherwise we ignore the UUID. The current assumption is that
			// such resources don't require persistent UUIDs. If that changes then this method needs to be updated.
			if(resourcesToSave.size() == 1)
			{
				Path outputPath = outputFolder + resourcesToSave[0].first;

				HResource resource = Importer::instance().import(filePath, resourcesToSave[0].second, UUID);
				if (resource != nullptr)
				{
					Resources::instance().save(resource, outputPath, true);
					manifest->registerResource(resource.getUUID(), outputPath);
				}

				return resource;
			}
			else
			{
				for (auto& entry : resourcesToSave)
				{
					Path outputPath = outputFolder + entry.first;;

					HResource resource = Importer::instance().import(filePath, entry.second);
					if (resource != nullptr)
					{
						Resources::instance().save(resource, outputPath, true);
						manifest->registerResource(resource.getUUID(), outputPath);
					}
				}
			}

			return HResource();
		};

		auto generateSprite = [&](const HTexture& texture, const String& fileName, const String& UUID)
		{
			Path relativePath = fileName;
			Path outputPath = spriteOutputFolder + relativePath;

			outputPath.setFilename("sprite_" + fileName + ".asset");

			SPtr<SpriteTexture> spriteTexPtr = SpriteTexture::_createPtr(texture);
			HResource spriteTex = gResources()._createResourceHandle(spriteTexPtr, UUID);

			Resources::instance().save(spriteTex, outputPath, true);
			manifest->registerResource(spriteTex.getUUID(), outputPath);
		};

		struct IconData
		{
			String name;
			HTexture source;
			SPtr<PixelData> srcData;
			std::string TextureUUIDs[3];
			std::string SpriteUUIDs[3];
		};

		Vector<IconData> iconsToGenerate;

		for(auto& entry : entries)
		{
			std::string name = entry["Path"];
			std::string uuid;

			bool isIcon = false;
			if (mode == AssetType::Normal)
			{
				uuid = entry["UUID"];
				isIcon = entry.find("UUID16") != entry.end();
			}
			else if (mode == AssetType::Sprite)
			{
				uuid = entry["TextureUUID"];
				isIcon = entry.find("TextureUUID16") != entry.end();
			}

			HResource outputRes = importResource(name.c_str(), uuid.c_str());
			if (outputRes == nullptr)
				continue;

			if (rtti_is_of_type<Shader>(outputRes.get()))
			{
				HShader shader = static_resource_cast<Shader>(outputRes);
				if (!verifyAndReportShader(shader))
					return false;
			}

			if (mode == AssetType::Sprite)
			{
				std::string spriteUUID = entry["SpriteUUID"];

				HTexture tex = static_resource_cast<Texture>(outputRes);
				generateSprite(tex, name.c_str(), spriteUUID.c_str());
			}

			if(isIcon)
			{
				IconData iconData;
				iconData.source = static_resource_cast<Texture>(outputRes);
				iconData.name = name.c_str();

				if (mode == AssetType::Normal)
				{
					iconData.TextureUUIDs[0] = entry["UUID48"];
					iconData.TextureUUIDs[1] = entry["UUID32"];
					iconData.TextureUUIDs[2] = entry["UUID16"];
				}
				else if (mode == AssetType::Sprite)
				{
					iconData.TextureUUIDs[0] = entry["TextureUUID48"];
					iconData.TextureUUIDs[1] = entry["TextureUUID32"];
					iconData.TextureUUIDs[2] = entry["TextureUUID16"];

					iconData.SpriteUUIDs[0] = entry["SpriteUUID48"];
					iconData.SpriteUUIDs[1] = entry["SpriteUUID32"];
					iconData.SpriteUUIDs[2] = entry["SpriteUUID16"];
				}

				iconsToGenerate.push_back(iconData);
			}
		}

		for(UINT32 i = 0; i < (UINT32)iconsToGenerate.size(); i++)
		{
			IconData& data = iconsToGenerate[i];

			data.srcData = data.source->getProperties().allocBuffer(0, 0);
			data.source->readData(data.srcData);
		}

		gCoreThread().submit(true);

		auto saveTexture = [&](auto& pixelData, auto& path, auto& UUID)
		{
			SPtr<Texture> texturePtr = Texture::_createPtr(pixelData);
			HResource texture = gResources()._createResourceHandle(texturePtr, UUID.c_str());

			Resources::instance().save(texture, path, true);
			manifest->registerResource(texture.getUUID(), path);

			return static_resource_cast<Texture>(texture);
		};

		for (UINT32 i = 0; i < (UINT32)iconsToGenerate.size(); i++)
		{
			SPtr<PixelData> src = iconsToGenerate[i].srcData;

			SPtr<PixelData> scaled48 = PixelData::create(48, 48, 1, src->getFormat());
			PixelUtil::scale(*src, *scaled48);

			SPtr<PixelData> scaled32 = PixelData::create(32, 32, 1, src->getFormat());
			PixelUtil::scale(*scaled48, *scaled32);

			SPtr<PixelData> scaled16 = PixelData::create(16, 16, 1, src->getFormat());
			PixelUtil::scale(*scaled32, *scaled16);

			Path outputPath48 = outputFolder + (iconsToGenerate[i].name + "48.asset");
			Path outputPath32 = outputFolder + (iconsToGenerate[i].name + "32.asset");
			Path outputPath16 = outputFolder + (iconsToGenerate[i].name + "16.asset");

			HTexture tex48 = saveTexture(scaled48, outputPath48, iconsToGenerate[i].TextureUUIDs[0]);
			HTexture tex32 = saveTexture(scaled32, outputPath32, iconsToGenerate[i].TextureUUIDs[1]);
			HTexture tex16 = saveTexture(scaled16, outputPath16, iconsToGenerate[i].TextureUUIDs[2]);

			if (mode == AssetType::Sprite)
			{
				generateSprite(tex48, iconsToGenerate[i].name + "48", iconsToGenerate[i].SpriteUUIDs[0].c_str());
				generateSprite(tex32, iconsToGenerate[i].name + "32", iconsToGenerate[i].SpriteUUIDs[1].c_str());
				generateSprite(tex16, iconsToGenerate[i].name + "16", iconsToGenerate[i].SpriteUUIDs[2].c_str());
			}
		}

		return true;
	}

	void BuiltinResourcesHelper::importFont(const Path& inputFile, const WString& outputName, const Path& outputFolder,
		const Vector<UINT32>& fontSizes, bool antialiasing, const String& UUID, const SPtr<ResourceManifest>& manifest)
	{
		SPtr<ImportOptions> fontImportOptions = Importer::instance().createImportOptions(inputFile);
		if (rtti_is_of_type<FontImportOptions>(fontImportOptions))
		{
			FontImportOptions* importOptions = static_cast<FontImportOptions*>(fontImportOptions.get());

			importOptions->setFontSizes(fontSizes);
			importOptions->setRenderMode(antialiasing ? FontRenderMode::HintedSmooth : FontRenderMode::HintedRaster);
		}
		else
			return;

		HFont font = Importer::instance().import<Font>(inputFile, fontImportOptions, UUID);

		WString fontName = outputName;
		Path outputPath = outputFolder + fontName;
		outputPath.setFilename(outputPath.getWFilename() + L".asset");

		Resources::instance().save(font, outputPath, true);
		manifest->registerResource(font.getUUID(), outputPath);

		// Save font texture pages as well. TODO - Later maybe figure out a more automatic way to do this
		for (auto& size : fontSizes)
		{
			SPtr<const FontBitmap> fontData = font->getBitmap(size);

			Path texPageOutputPath = outputFolder;

			UINT32 pageIdx = 0;
			for (auto tex : fontData->texturePages)
			{
				texPageOutputPath.setFilename(fontName + L"_" + toWString(size) + L"_texpage_" +
					toWString(pageIdx) + L".asset");

				Resources::instance().save(tex, texPageOutputPath, true);
				manifest->registerResource(tex.getUUID(), texPageOutputPath);
			}
		}
	}

	bool BuiltinResourcesHelper::updateJSON(const Path& folder, AssetType type, nlohmann::json& entries)
	{
		UnorderedSet<Path> existingEntries;
		for(auto& entry : entries)
		{
			std::string strPath = entry["Path"];
			Path path = strPath.c_str();

			existingEntries.insert(path);
		}

		bool foundChanges = false;
		auto checkForChanges = [&](const Path& filePath)
		{
			Path relativePath = filePath.getRelative(folder);

			auto iterFind = existingEntries.find(relativePath);
			if(iterFind == existingEntries.end())
			{
				if(type == AssetType::Normal)
				{
					String uuid = UUIDGenerator::generateRandom();
					nlohmann::json newEntry =
					{ 
						{ "Path", relativePath.toString().c_str() },
						{ "UUID", uuid.c_str() } 
					};

					entries.push_back(newEntry);
				}
				else // Sprite
				{
					String texUuid = UUIDGenerator::generateRandom();
					String spriteUuid = UUIDGenerator::generateRandom();
					nlohmann::json newEntry = 
					{ 
						{ "Path", relativePath.toString().c_str() },
						{ "SpriteUUID", spriteUuid.c_str() },
						{ "TextureUUID", texUuid.c_str() }
					};

					entries.push_back(newEntry);
				}

				foundChanges = true;
			}

			return true;
		};

		FileSystem::iterate(folder, checkForChanges, nullptr);

		// Prune deleted entries
		auto iter = entries.begin();
		while(iter != entries.end())
		{
			std::string strPath = (*iter)["Path"];
			Path path = strPath.c_str();
			path = path.getAbsolute(folder);

			if (!FileSystem::exists(path))
			{
				iter = entries.erase(iter);
				foundChanges = true;
			}
			else
				++iter;
		}

		return foundChanges;
	}

	void BuiltinResourcesHelper::writeTimestamp(const Path& file)
	{
		SPtr<DataStream> fileStream = FileSystem::createAndOpenFile(file);

		time_t currentTime = std::time(nullptr);
		fileStream->write(&currentTime, sizeof(currentTime));
		fileStream->close();
	}

	bool BuiltinResourcesHelper::checkForModifications(const Path& folder, const Path& timeStampFile)
	{
		if (!FileSystem::exists(timeStampFile))
			return true;

		SPtr<DataStream> fileStream = FileSystem::openFile(timeStampFile);
		time_t lastUpdateTime = 0;
		fileStream->read(&lastUpdateTime, sizeof(lastUpdateTime));
		fileStream->close();

		bool upToDate = true;
		auto checkUpToDate = [&](const Path& filePath)
		{
			time_t fileLastModified = FileSystem::getLastModifiedTime(filePath);

			if (fileLastModified > lastUpdateTime)
			{
				upToDate = false;
				return false;
			}

			return true;
		};

		FileSystem::iterate(folder, checkUpToDate, checkUpToDate);

		return !upToDate;
	}

	bool BuiltinResourcesHelper::verifyAndReportShader(const HShader& shader)
	{
		if(!shader.isLoaded(false) || shader->getNumTechniques() == 0)
		{
#if BS_DEBUG_MODE
			BS_EXCEPT(InvalidStateException, "Error occured while compiling a shader. Check earlier log messages for exact error.");
#else
			LOGERR("Error occured while compiling a shader. Check earlier log messages for exact error.")
#endif
			return false;
		}

		Vector<SPtr<Technique>> techniques = shader->getCompatibleTechniques();
		for(auto& technique : techniques)
		{
			UINT32 numPasses = technique->getNumPasses();
			for(UINT32 i = 0; i < numPasses; i++)
			{
				SPtr<Pass> pass = technique->getPass(i);

				std::array<SPtr<GpuProgram>, 6> gpuPrograms;
				gpuPrograms[0] = pass->getVertexProgram();
				gpuPrograms[1] = pass->getFragmentProgram();
				gpuPrograms[2] = pass->getGeometryProgram();
				gpuPrograms[3] = pass->getHullProgram();
				gpuPrograms[4] = pass->getDomainProgram();
				gpuPrograms[5] = pass->getComputeProgram();

				for(auto& program : gpuPrograms)
				{
					if (program == nullptr)
						continue;

					program->blockUntilCoreInitialized();
					if(!program->isCompiled())
					{
#if BS_DEBUG_MODE
						BS_EXCEPT(InvalidStateException, "Error occured while compiling a shader. Error message: " + 
							program->getCompileErrorMessage());
#else
						LOGERR("Error occured while compiling a shader. Error message: " +
							program->getCompileErrorMessage())
#endif
						return false;
					}
				}
			}
		}

		return true;
	}
}