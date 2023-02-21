#include "DataModule.h"
#include "PresetMan.h"
#include "SceneMan.h"
#include "LuaMan.h"

namespace RTE {

	const std::string DataModule::c_ClassName = "DataModule";

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void DataModule::Clear() {
		m_IsUserdata = false;
		m_FileName.clear();
		m_FriendlyName.clear();
		m_Author.clear();
		m_Description.clear();
		m_SupportedGameVersion.clear();
		m_Version = 1;
		m_ModuleID = -1;
		m_IconFile.Reset();
		m_Icon = nullptr;
		m_PresetList.clear();
		m_EntityList.clear();
		m_TypeMap.clear();
		m_MaterialMappings.fill(0);
		m_ScanFolderContents = false;
		m_IgnoreMissingItems = false;
		m_CrabToHumanSpawnRatio = 0;
		m_ScriptPath.clear();
		m_IsFaction = false;
		m_IsMerchant = false;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	int DataModule::Create(const std::string &moduleName, const ProgressCallback &progressCallback) {
		m_FileName = std::filesystem::path(moduleName).generic_string();
		m_ModuleID = g_PresetMan.GetModuleID(moduleName);
		m_CrabToHumanSpawnRatio = 0;

		// Report that we're starting to read a new DataModule
		if (progressCallback) { progressCallback(m_FileName + " " + static_cast<char>(-43) + " loading:", true); }

		Reader reader;
		std::string indexPath = g_PresetMan.FullModulePath(m_FileName + "/Index.ini");
		std::string mergedIndexPath = g_PresetMan.FullModulePath(m_FileName + "/MergedIndex.ini");

		// NOTE: This looks for the MergedIndex.ini generated by the index merger tool. The tool is mostly superseded by disabling loading visuals, but still provides some benefit.
		if (std::filesystem::exists(mergedIndexPath)) { indexPath = mergedIndexPath; }

		if (reader.Create(indexPath, true, progressCallback) >= 0) {
			int result = Serializable::Create(reader);

			if (m_ModuleID >= g_PresetMan.GetOfficialModuleCount() && !m_IsUserdata && m_SupportedGameVersion != c_GameVersion) {
				RTEAssert(!m_SupportedGameVersion.empty(), m_FileName + " does not specify a supported Cortex Command version, so it is not compatible with this version of Cortex Command (" + c_GameVersion + ").\nPlease contact the mod author or ask for help in the CCCP discord server.");
				RTEAbort(m_FileName + " supports Cortex Command version " + m_SupportedGameVersion + ", so it is not compatible with this version of Cortex Command (" + c_GameVersion + ").\nPlease contact the mod author or ask for help in the CCCP discord server.");
			}

			// Print an empty line to separate the end of a module from the beginning of the next one in the loading progress log.
			if (progressCallback) { progressCallback(" ", true); }

			if (m_ScanFolderContents) { result = FindAndRead(progressCallback); }

			return result;
		}
		return -1;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	bool DataModule::CreateOnDiskAsUserdata(const std::string &moduleName, const std::string_view &friendlyName, bool ignoreMissingItems, bool scanFolderContents) {
		std::string moduleNameWithPackageExtension = System::GetUserdataDirectory() + moduleName + ((moduleName.ends_with(System::GetModulePackageExtension()) ? "" : System::GetModulePackageExtension()));
		if (Writer writer(moduleNameWithPackageExtension + "/Index.ini", false, true); writer.WriterOK()) {
			DataModule newModule;
			newModule.m_IsUserdata = true;
			newModule.m_FriendlyName = friendlyName;
			newModule.m_IgnoreMissingItems = ignoreMissingItems;
			newModule.m_ScanFolderContents = scanFolderContents;
			newModule.Save(writer);
			writer.EndWrite();
		} else {
			return false;
		}
		return true;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void DataModule::Destroy() {
		for (const PresetEntry &preset : m_PresetList){
			delete preset.m_EntityPreset;
		}
		Clear();
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	int DataModule::ReadModuleProperties(const std::string &moduleName, const ProgressCallback &progressCallback) {
		m_FileName = moduleName;
		m_ModuleID = g_PresetMan.GetModuleID(moduleName);
		m_CrabToHumanSpawnRatio = 0;

		// Report that we're starting to read a new DataModule
		if (progressCallback) { progressCallback(m_FileName + " " + static_cast<char>(-43) + " reading properties:", true); }
		Reader reader;
		std::string indexPath(m_FileName + "/Index.ini");

		if (reader.Create(indexPath, true, progressCallback) >= 0) {
			reader.SetSkipIncludes(true);
			int result = Serializable::Create(reader);
			return result;
		}
		return -1;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	int DataModule::ReadProperty(const std::string_view &propName, Reader &reader) {
		if (propName == "ModuleName") {
			reader >> m_FriendlyName;
		} else if (propName == "Author") {
			reader >> m_Author;
		} else if (propName == "Description") {
			std::string descriptionValue = reader.ReadPropValue();
			if (descriptionValue == "MultiLineText") {
				m_Description.clear();
				while (reader.NextProperty() && reader.ReadPropName() == "AddLine") {
					m_Description += reader.ReadPropValue() + "\n\n";
				}
				if (!m_Description.empty()) {
					m_Description.resize(m_Description.size() - 2);
				}
			} else {
				m_Description = descriptionValue;
			}
		} else if (propName == "IsFaction") {
			reader >> m_IsFaction;
			if (m_IsMerchant) { m_IsFaction = false; }
		} else if (propName == "IsMerchant") {
			reader >> m_IsMerchant;
			if (m_IsMerchant) { m_IsFaction = false; }
		} else if (propName == "SupportedGameVersion") {
			reader >> m_SupportedGameVersion;
		} else if (propName == "Version") {
			reader >> m_Version;
		} else if (propName == "ScanFolderContents") {
			reader >> m_ScanFolderContents;
		} else if (propName == "IgnoreMissingItems") {
			reader >> m_IgnoreMissingItems;
		} else if (propName == "CrabToHumanSpawnRatio") {
			reader >> m_CrabToHumanSpawnRatio;
		} else if (propName == "ScriptPath") {
			reader >> m_ScriptPath;
			LoadScripts();
		} else if (propName == "Require") {
			// Check for required dependencies if we're not load properties
			std::string requiredModule;
			reader >> requiredModule;
			if (!reader.GetSkipIncludes() && g_PresetMan.GetModuleID(requiredModule) == -1) {
				reader.ReportError("\"" + m_FileName + "\" requires \"" + requiredModule + "\" in order to load!\n");
			}
		} else if (propName == "IconFile") {
			reader >> m_IconFile;
			m_Icon = m_IconFile.GetAsBitmap();
		} else if (propName == "FactionBuyMenuTheme") {
			if (reader.ReadPropValue() == "BuyMenuTheme") {
				while (reader.NextProperty()) {
					std::string themePropName = reader.ReadPropName();
					if (themePropName == "SkinFile") {
						m_BuyMenuTheme.SkinFilePath = reader.ReadPropValue();
					} else if (themePropName == "BannerFile") {
						m_BuyMenuTheme.BannerImagePath = reader.ReadPropValue();
					} else if (themePropName == "LogoFile") {
						m_BuyMenuTheme.LogoImagePath = reader.ReadPropValue();
					} else if (themePropName == "BackgroundColorIndex") {
						m_BuyMenuTheme.BackgroundColorIndex = std::clamp(std::stoi(reader.ReadPropValue()), 0, 255);
					} else {
						break;
					}
				}
			}
		} else if (propName == "AddMaterial") {
			return g_SceneMan.ReadProperty(propName, reader);
		} else {
			// Try to read in the preset and add it to the PresetMan in one go, and report if fail
			if (!g_PresetMan.GetEntityPreset(reader)) { reader.ReportError("Could not understand Preset type!"); }
		}
		return 0;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	int DataModule::Save(Writer &writer) const {
		Serializable::Save(writer);

		writer.NewPropertyWithValue("ModuleName", m_FriendlyName);

		if (!m_IsUserdata) {
			writer.NewPropertyWithValue("Author", m_Author);
			writer.NewPropertyWithValue("Description", m_Description);
			writer.NewPropertyWithValue("IsFaction", m_IsFaction);
			writer.NewPropertyWithValue("SupportedGameVersion", m_SupportedGameVersion);
			writer.NewPropertyWithValue("Version", m_Version);
			writer.NewPropertyWithValue("IconFile", m_IconFile);

			// TODO: Write out all the different entity instances, each having their own relative location within the data module stored
			// Will need the writer to be able to open different files and append to them as needed, probably done in NewEntity()
			// writer.NewEntity()
		} else {
			writer.NewPropertyWithValue("ScanFolderContents", m_ScanFolderContents);
			writer.NewPropertyWithValue("IgnoreMissingItems", m_IgnoreMissingItems);
		}

		return 0;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	std::string DataModule::GetEntityDataLocation(const std::string &exactType, const std::string &instance) {
		const Entity *foundEntity = GetEntityPreset(exactType, instance);
		if (foundEntity == nullptr) {
			return "";
		}

		// Search for entity in instanceList
		for (const PresetEntry &presetListEntry : m_PresetList) {
			if (presetListEntry.m_EntityPreset == foundEntity) {
				return presetListEntry.m_FileReadFrom;
			}
		}

		RTEAbort("Tried to find allegedly existing Entity Preset Entry: " + foundEntity->GetPresetName() + ", but couldn't!");
		return "";
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	const Entity * DataModule::GetEntityPreset(const std::string &exactType, const std::string &instance) {
		if (exactType.empty() || instance == "None" || instance.empty()) {
			return nullptr;
		}
		if (auto classItr = m_TypeMap.find(exactType); classItr != m_TypeMap.end()) {
			// Find an instance of that EXACT type and name; derived types are not matched
			for (const auto &[instanceName, entity] : classItr->second) {
				if (instanceName == instance && entity->GetClassName() == exactType) {
					return entity;
				}
			}
		}
		return nullptr;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	bool DataModule::AddEntityPreset(Entity *entityToAdd, bool overwriteSame, const std::string &readFromFile) {
		// Fail if the entity is unnamed or it's not the original preset.
		//TODO If we're overwriting, we may not want to fail if it's not the original preset, this needs to be investigated
		if (entityToAdd->GetPresetName() == "None" || entityToAdd->GetPresetName().empty() || !entityToAdd->IsOriginalPreset()) {
			return false;
		}
		bool entityAdded = false;

		if (Entity *existingEntity = GetEntityIfExactType(entityToAdd->GetClassName(), entityToAdd->GetPresetName())) {
			// If we're commanded to overwrite any collisions, then do so by cloning over the existing instance in the list
			// This way we're not invalidating any instance references that would have been taken out and held by clients
			if (overwriteSame) {
				entityToAdd->SetModuleID(m_ModuleID); //TODO this is probably overwritten by Entity::Create(other), making it useless. Double-check this and remove this line if certain
				entityToAdd->Clone(existingEntity);
				// Make sure the existing one is still marked as the Original Preset
				existingEntity->m_IsOriginalPreset = true;
				// Alter the instance entry to reflect the data file location of the new definition
				if (readFromFile != "Same") {
					std::list<PresetEntry>::iterator itr = m_PresetList.begin();
					for (; itr != m_PresetList.end(); ++itr) {
						// When we find the correct entry, alter its data file location
						if ((*itr).m_EntityPreset == existingEntity) {
							(*itr).m_FileReadFrom = readFromFile;
							break;
						}
					}
					RTEAssert(itr != m_PresetList.end(), "Tried to alter allegedly existing Entity Preset Entry: " + entityToAdd->GetPresetName() + ", but couldn't find it in the list!");
				}
				return true;
			} else {
				return false;
			}
		} else {
			entityToAdd->SetModuleID(m_ModuleID);
			Entity *entityClone = entityToAdd->Clone();
			// Mark the one we are about to add to the list as the Original now - this is now the actual Original Preset instance
			entityClone->m_IsOriginalPreset = true;

			if (readFromFile == "Same" && m_PresetList.empty()) {
				RTEAbort("Tried to add first entity instance to data module " + m_FileName + " without specifying a data file!");
			}

			m_PresetList.push_back(PresetEntry(entityClone, readFromFile != "Same" ? readFromFile : m_PresetList.back().m_FileReadFrom));
			m_EntityList.push_back(entityClone);
			entityAdded = AddToTypeMap(entityClone);
			RTEAssert(entityAdded, "Unexpected problem while adding Entity instance \"" + entityToAdd->GetPresetName() + "\" to the type map of data module: " + m_FileName);
		}
		return entityAdded;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	bool DataModule::GetGroupsWithType(std::list<std::string> &groupList, const std::string &withType) {
		bool foundAny = false;

		if (withType == "All" || withType.empty()) {
			for (const std::string &groupRegisterEntry : m_GroupRegister) {
				groupList.push_back(groupRegisterEntry);
				// TODO: it seems weird that foundAny isn't set to true here, given that the list gets filled.
				// But I suppose no actual finding is done. Investigate this and see where it's called, maybe this should be changed
			}
		} else {
			if (auto classItr = m_TypeMap.find(withType); classItr != m_TypeMap.end()) {
				const std::list<std::string> *groupListPtr = nullptr;
				// Go through all the entities of that type, adding the groups they belong to
				for (const auto &[instanceName, entity] : classItr->second) {
					groupListPtr = entity->GetGroupList();

					for (const std::string &groupListEntry : *groupListPtr) {
						groupList.push_back(groupListEntry); // Get the grouped entities, without transferring ownership
						foundAny = true;
					}
				}

				// Make sure there are no dupe groups in the list
				groupList.sort();
				groupList.unique();
			}
		}
		return foundAny;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


	bool DataModule::GetAllOfOrNotOfGroups(std::list<Entity *> &entityList, const std::string &type, const std::vector<std::string> &groups, bool excludeGroups) {
		if (groups.empty()) {
			return false;
		}
		bool foundAny = false;

		// Find either the Entity typelist that contains all entities in this DataModule, or the specific class' typelist (which will get all derived classes too).
		if (auto classItr = m_TypeMap.find((type.empty() || type == "All") ? "Entity" : type); classItr != m_TypeMap.end()) {
			RTEAssert(!classItr->second.empty(), "DataModule has class entry without instances in its map!?");

			for (const auto &[instanceName, entity] : classItr->second) {
				if (excludeGroups) {
					bool excludeEntity = false;
					for (const std::string &group : groups) {
						if (entity->IsInGroup(group)) {
							excludeEntity = true;
							break;
						}
					}
					if (!excludeEntity) {
						entityList.emplace_back(entity);
						foundAny = true;
					}
				} else {
					for (const std::string &group : groups) {
						if (entity->IsInGroup(group)) {
							entityList.emplace_back(entity);
							foundAny = true;
							break;
						}
					}
				}
			}
		}
		return foundAny;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	bool DataModule::GetAllOfType(std::list<Entity *> &entityList, const std::string &type) {
		if (type.empty()) {
			return false;
		}

		if (auto classItr = m_TypeMap.find(type);  classItr != m_TypeMap.end()) {
			RTEAssert(!classItr->second.empty(), "DataModule has class entry without instances in its map!?");

			for (const auto &[instanceName, entity] : classItr->second) {
				entityList.push_back(entity); // Get the entities, without transferring ownership
			}
			return true;
		}
		return false;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	bool DataModule::AddMaterialMapping(unsigned char fromID, unsigned char toID) {
		RTEAssert(fromID > 0 && fromID < c_PaletteEntriesNumber && toID > 0 && toID < c_PaletteEntriesNumber, "Tried to make an out-of-bounds Material mapping");

		bool clear = m_MaterialMappings.at(fromID) == 0;
		m_MaterialMappings.at(fromID) = toID;

		return clear;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	int DataModule::LoadScripts() const {
		return m_ScriptPath.empty() ? 0 : g_LuaMan.RunScriptFile(m_ScriptPath);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void DataModule::ReloadAllScripts() const {
		for (const PresetEntry &presetListEntry : m_PresetList) {
			presetListEntry.m_EntityPreset->ReloadScripts();
		}
		LoadScripts();
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	int DataModule::FindAndRead(const ProgressCallback &progressCallback) {
		int result = 0;
		const std::string directoryToScan = g_PresetMan.FullModulePath(m_FileName);
		for (const std::filesystem::directory_entry &directoryEntry : std::filesystem::directory_iterator(System::GetWorkingDirectory() + directoryToScan)) {
			if (directoryEntry.path().extension() == ".ini" && directoryEntry.path().filename() != "Index.ini") {
				Reader iniReader;
				if (iniReader.Create(m_FileName + "/" + directoryEntry.path().filename().generic_string(), false, progressCallback) >= 0) {
					result = Serializable::CreateSerializable(iniReader, false, true, true);
					if (progressCallback) { progressCallback(" ", true); }
				}
			}
		}
		return result;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// TODO: This method is almost identical to GetEntityPreset, except it doesn't return a const Entity *.
	// Investigate if the latter needs to return const (based on what's using it) and if not, get rid of this and replace its uses. At the very least, consider renaming this
	// See https://github.com/cortex-command-community/Cortex-Command-Community-Project-Source/issues/87
	Entity * DataModule::GetEntityIfExactType(const std::string &exactType, const std::string &presetName) {
		if (exactType.empty() || presetName == "None" || presetName.empty()) {
			return nullptr;
		}
		if (auto classItr = m_TypeMap.find(exactType); classItr != m_TypeMap.end()) {
			// Find an instance of that EXACT type and name; derived types are not matched
			for (const auto &[instanceName, entity] : classItr->second) {
				if (instanceName == presetName && entity->GetClassName() == exactType) {
					return entity;
				}
			}
		}
		return nullptr;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	bool DataModule::AddToTypeMap(Entity *entityToAdd) {
		if (!entityToAdd || entityToAdd->GetPresetName() == "None" || entityToAdd->GetPresetName().empty()) {
			return false;
		}

		// Walk up the class hierarchy till we reach the top, adding an entry of the passed in entity into each typelist as we go along
		for (const Entity::ClassInfo *pClass = &(entityToAdd->GetClass()); pClass != nullptr; pClass = pClass->GetParent()) {
			auto classItr = m_TypeMap.find(pClass->GetName());

			// No instances of this entity have been added yet so add a class category for it
			if (classItr == m_TypeMap.end()) {
				classItr = (m_TypeMap.insert(std::pair<std::string, std::list<std::pair<std::string, Entity *>>>(pClass->GetName(), std::list<std::pair<std::string, Entity *>>()))).first;
			}

			// NOTE We're adding the entity to the class category list but not transferring ownership. Also, we're not checking for collisions as they're assumed to have been checked for already
			(*classItr).second.push_back(std::pair<std::string, Entity *>(entityToAdd->GetPresetName(), entityToAdd));
		}
		return true;
	}
}