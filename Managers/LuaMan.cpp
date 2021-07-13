#include "LuaMan.h"

#define BOOST_BIND_GLOBAL_PLACEHOLDERS 1

#include "lua.hpp"

#include "LuaBindingsManagers.h"
#include "LuaBindingsActivities.h"
#include "LuaBindingsGUI.h"

/// <summary>
/// Special callback function for adding file name and line number to error messages when calling functions incorrectly.
/// </summary>
/// <param name="pState">The Lua master state.</param>
/// <returns>An error signal, 1, so Lua correctly reports that there's been an error.</returns>
int AddFileAndLineToError(lua_State *pState) {
    lua_Debug luaDebug;
    if (lua_getstack(pState, 2, &luaDebug) > 0) {
        lua_getinfo(pState, "Sln", &luaDebug);
        std::string errorString = lua_tostring(pState, -1);

        if (errorString.find(".lua") != std::string::npos) {
            lua_pushstring(pState, errorString.c_str());
        } else {
            std::stringstream messageStream;
            messageStream << ((luaDebug.name == nullptr || strstr(luaDebug.name, ".rte") == nullptr) ? luaDebug.short_src : luaDebug.name);
            messageStream << ":" << luaDebug.currentline << ": " << errorString;
            lua_pushstring(pState, messageStream.str().c_str());
        }
    }
   return 1;
}






namespace RTE {

	/// <summary>
	/// Explicit deletion of any Entity instance that Lua owns. It will probably be handled by the GC, but this makes it instantaneous.
	/// </summary>
	/// <param name="entityToDelete">The Entity to delete.</param>
	void DeleteEntity(Entity *entityToDelete) {
		delete entityToDelete;
		entityToDelete = nullptr;
	}


	//////////////////////////////////////////////////////////////////////////////////////////
	// Other misc adapters to eliminate/emulate default parameters etc

	double NormalRand() { return RandomNormalNum<double>(); }
	double PosRand() { return RandomNum<double>(); }
	double LuaRand(double num) { return RandomNum<double>(1, num); }

	/*
	//////////////////////////////////////////////////////////////////////////////////////////
	// Wrapper for the GAScripted so we can derive new classes from it purely in lua:
	//
	// "It is also possible to derive Lua classes from C++ classes, and override virtual functions with Lua functions.
	// To do this we have to create a wrapper class for our C++ base class.
	// This is the class that will hold the Lua object when we instantiate a Lua class"

	struct GAScriptedWrapper:
		GAScripted,
		wrap_base
	{
		GAScriptedWrapper(): GAScripted() { ; }

		// Passing in the path of the script file that defines, in Lua, the GAScripted-derived class
	//    virtual int Create(const GAScripted &reference) { return call<int>("Create", reference); }
		virtual Entity * Clone(Entity *pCloneTo = 0) const { return call<Entity *>("Clone", pCloneTo); }
		virtual int Start() { call<int>("Start"); }
		virtual void Pause(bool pause) { call<void>("Pause", pause); }
		virtual void End() { call<void>("End"); }
		virtual void Update() { call<void>("Update"); }

	//    static int static_Create(GAScripted *pSelf, const GAScripted &reference) { return pSelf->GAScripted::Create(reference); }
		static Entity * static_Clone(GAScripted *pSelf, Entity *pCloneTo = 0) { return pSelf->GAScripted::Clone(pCloneTo); }
		static int static_Start(GAScripted *pSelf) { return pSelf->GAScripted::Start(); }
		static void static_Pause(GAScripted *pSelf, bool pause) { return pSelf->GAScripted::Pause(pause); }
		static void static_End(GAScripted *pSelf) { return pSelf->GAScripted::End(); }
		static void static_Update(GAScripted *pSelf) { return pSelf->GAScripted::Update(); }
	};
	*/











/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void LuaMan::Clear() {
		m_MasterState = nullptr;
		m_LastError.clear();
		// TODO: is this the best way to give ID's. won't ever be reset?
		m_NextPresetID = 0;
		m_NextObjectID = 0;

		m_TempEntity = nullptr;
		m_TempEntityVector.clear();
		m_TempEntityVector.shrink_to_fit();

		m_OpenedFiles.fill(nullptr);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void LuaMan::Initialize() {
		m_MasterState = luaL_newstate();
		// Attach the master state to LuaBind
		luabind::open(m_MasterState);

		const luaL_Reg libsToLoad[] = {
			{ LUA_COLIBNAME, luaopen_base },
			{ LUA_LOADLIBNAME, luaopen_package },
			{ LUA_TABLIBNAME, luaopen_table },
			{ LUA_STRLIBNAME, luaopen_string },
			{ LUA_MATHLIBNAME, luaopen_math },
			{ LUA_DBLIBNAME, luaopen_debug },
			{ LUA_JITLIBNAME, luaopen_jit },
			{ NULL, NULL } // End of array
		};

		for (const luaL_Reg *lib = libsToLoad; lib->func; lib++) {
			lua_pushcfunction(m_MasterState, lib->func);
			lua_pushstring(m_MasterState, lib->name);
			lua_call(m_MasterState, 1, 0);
		}

		// LuaJIT should start automatically after we load the library but we're making sure it did anyway.
		if (!luaJIT_setmode(m_MasterState, 0, LUAJIT_MODE_ENGINE | LUAJIT_MODE_ON)) { RTEAbort("Failed to initialize LuaJIT!"); }

		// From LuaBind documentation:
		// As mentioned in the Lua documentation, it is possible to pass an error handler function to lua_pcall().
		// Luabind makes use of lua_pcall() internally when calling member functions and free functions.
		// It is possible to set the error handler function that Luabind will use globally:
		//set_pcall_callback(&AddFileAndLineToError); //NOTE: This seems to do nothing

		// Register all relevant bindings to the master state. Note that the order of registration is important, as bindings can't derive from an unregistered type (inheritance and all that).
		luabind::module(m_MasterState)[
			SystemLuaBindings::RegisterVectorLuaBindings(),
			SystemLuaBindings::RegisterBoxLuaBindings(),
			EntityLuaBindings::RegisterSceneAreaLuaBindings(),
			EntityLuaBindings::RegisterEntityLuaBindings(),
			RegisterLuaBindingsOfConcreteType(EntityLuaBindings, SoundContainer),
			EntityLuaBindings::RegisterSoundSetLuaBindings(),
			EntityLuaBindings::RegisterLimbPathLuaBindings(),
			RegisterLuaBindingsOfAbstractType(EntityLuaBindings, SceneObject),
			RegisterLuaBindingsOfAbstractType(EntityLuaBindings, MovableObject),
			EntityLuaBindings::RegisterMaterialLuaBindings(),
			RegisterLuaBindingsOfConcreteType(EntityLuaBindings, MOPixel),
			RegisterLuaBindingsOfConcreteType(EntityLuaBindings, TerrainObject),
			RegisterLuaBindingsOfAbstractType(EntityLuaBindings, MOSprite),
			RegisterLuaBindingsOfConcreteType(EntityLuaBindings, MOSParticle),
			RegisterLuaBindingsOfConcreteType(EntityLuaBindings, MOSRotating),
			RegisterLuaBindingsOfConcreteType(EntityLuaBindings, Attachable),
			RegisterLuaBindingsOfAbstractType(EntityLuaBindings, Emission),
			RegisterLuaBindingsOfConcreteType(EntityLuaBindings, AEmitter),
			RegisterLuaBindingsOfConcreteType(EntityLuaBindings, PEmitter),
			RegisterLuaBindingsOfConcreteType(EntityLuaBindings, Actor),
			RegisterLuaBindingsOfConcreteType(EntityLuaBindings, ADoor),
			RegisterLuaBindingsOfConcreteType(EntityLuaBindings, Arm),
			RegisterLuaBindingsOfConcreteType(EntityLuaBindings, Leg),
			RegisterLuaBindingsOfConcreteType(EntityLuaBindings, AHuman),
			RegisterLuaBindingsOfConcreteType(EntityLuaBindings, ACrab),
			RegisterLuaBindingsOfConcreteType(EntityLuaBindings, Turret),
			RegisterLuaBindingsOfAbstractType(EntityLuaBindings, ACraft),
			RegisterLuaBindingsOfConcreteType(EntityLuaBindings, ACDropShip),
			RegisterLuaBindingsOfConcreteType(EntityLuaBindings, ACRocket),
			RegisterLuaBindingsOfConcreteType(EntityLuaBindings, HeldDevice),
			RegisterLuaBindingsOfConcreteType(EntityLuaBindings, Magazine),
			RegisterLuaBindingsOfConcreteType(EntityLuaBindings, Round),
			RegisterLuaBindingsOfConcreteType(EntityLuaBindings, HDFirearm),
			RegisterLuaBindingsOfConcreteType(EntityLuaBindings, ThrownDevice),
			RegisterLuaBindingsOfConcreteType(EntityLuaBindings, TDExplosive),
			SystemLuaBindings::RegisterControllerLuaBindings(),
			SystemLuaBindings::RegisterTimerLuaBindings(),
			RegisterLuaBindingsOfConcreteType(EntityLuaBindings, Scene),
			RegisterLuaBindingsOfAbstractType(EntityLuaBindings, Deployment),
			SystemLuaBindings::RegisterDataModuleLuaBindings(),
			ActivityLuaBindings::RegisterActivityLuaBindings(),
			RegisterLuaBindingsOfAbstractType(ActivityLuaBindings, GameActivity),
			SystemLuaBindings::RegisterPieSliceLuaBindings(),
			RegisterLuaBindingsOfAbstractType(EntityLuaBindings, GlobalScript),
			EntityLuaBindings::RegisterMetaPlayerLuaBindings(),
			GUILuaBindings::RegisterGUIBannerLuaBindings(),
			GUILuaBindings::RegisterBuyMenuGUILuaBindings(),
			GUILuaBindings::RegisterSceneEditorGUILuaBindings(),
			ManagerLuaBindings::RegisterActivityManLuaBindings(),
			ManagerLuaBindings::RegisterAudioManLuaBindings(),
			ManagerLuaBindings::RegisterConsoleManLuaBindings(),
			ManagerLuaBindings::RegisterFrameManLuaBindings(),
			ManagerLuaBindings::RegisterMetaManLuaBindings(),
			ManagerLuaBindings::RegisterMovableManLuaBindings(),
			ManagerLuaBindings::RegisterPostProcessManLuaBindings(),
			ManagerLuaBindings::RegisterPresetManLuaBindings(),
			ManagerLuaBindings::RegisterPrimitiveManLuaBindings(),
			ManagerLuaBindings::RegisterSceneManLuaBindings(),
			ManagerLuaBindings::RegisterSettingsManLuaBindings(),
			ManagerLuaBindings::RegisterTimerManLuaBindings(),
			ManagerLuaBindings::RegisterUInputManLuaBindings(),

			luabind::class_<AlarmEvent>("AlarmEvent")
				.def(luabind::constructor<>())
				.def(luabind::constructor<const Vector &, int, float>())
				.def_readwrite("ScenePos", &AlarmEvent::m_ScenePos)
				.def_readwrite("Team", &AlarmEvent::m_Team)
				.def_readwrite("Range", &AlarmEvent::m_Range),

			luabind::class_<LuaMan>("LuaManager")
				.property("TempEntity", &LuaMan::GetTempEntity)
				.def_readonly("TempEntities", &LuaMan::m_TempEntityVector, luabind::return_stl_iterator)
				.def("FileOpen", &LuaMan::FileOpen)
				.def("FileClose", &LuaMan::FileClose)
				.def("FileReadLine", &LuaMan::FileReadLine)
				.def("FileWriteLine", &LuaMan::FileWriteLine)
				.def("FileEOF", &LuaMan::FileEOF),

			// NOT a member function, so adopting _1 instead of the _2 for the first param, since there's no "this" pointer!!
			luabind::def("DeleteEntity", &DeleteEntity, luabind::adopt(_1)),
			luabind::def("RangeRand", (double(*)(double, double)) &RandomNum),
			luabind::def("PosRand", &PosRand),
			luabind::def("NormalRand", &NormalRand),
			luabind::def("SelectRand", (int(*)(int, int)) &RandomNum),
			luabind::def("LERP", &LERP),
			luabind::def("EaseIn", &EaseIn),
			luabind::def("EaseOut", &EaseOut),
			luabind::def("EaseInOut", &EaseInOut),
			luabind::def("Clamp", &Limit),
			luabind::def("GetPPM", &GetPPM),
			luabind::def("GetMPP", &GetMPP),
			luabind::def("GetPPL", &GetPPL),
			luabind::def("GetLPP", &GetLPP),
			luabind::def("RoundFloatToPrecision", &RoundFloatToPrecision),

			luabind::class_<input_device>("InputDevice")
			.enum_("InputDevice")[
				luabind::value("DEVICE_KEYB_ONLY", InputDevice::DEVICE_KEYB_ONLY),
				luabind::value("DEVICE_MOUSE_KEYB", InputDevice::DEVICE_MOUSE_KEYB),
				luabind::value("DEVICE_GAMEPAD_1", InputDevice::DEVICE_GAMEPAD_1),
				luabind::value("DEVICE_GAMEPAD_2", InputDevice::DEVICE_GAMEPAD_2),
				luabind::value("DEVICE_GAMEPAD_3", InputDevice::DEVICE_GAMEPAD_3),
				luabind::value("DEVICE_GAMEPAD_4", InputDevice::DEVICE_GAMEPAD_4),
				luabind::value("DEVICE_COUNT", InputDevice::DEVICE_COUNT)
			],

			luabind::class_<input_elements>("InputElements")
			.enum_("InputElements")[
				luabind::value("INPUT_L_UP", InputElements::INPUT_L_UP),
				luabind::value("INPUT_L_DOWN", InputElements::INPUT_L_DOWN),
				luabind::value("INPUT_L_LEFT", InputElements::INPUT_L_LEFT),
				luabind::value("INPUT_L_RIGHT", InputElements::INPUT_L_RIGHT),
				luabind::value("INPUT_R_UP", InputElements::INPUT_R_UP),
				luabind::value("INPUT_R_DOWN", InputElements::INPUT_R_DOWN),
				luabind::value("INPUT_R_LEFT", InputElements::INPUT_R_LEFT),
				luabind::value("INPUT_R_RIGHT", InputElements::INPUT_R_RIGHT),
				luabind::value("INPUT_FIRE", InputElements::INPUT_FIRE),
				luabind::value("INPUT_AIM", InputElements::INPUT_AIM),
				luabind::value("INPUT_AIM_UP", InputElements::INPUT_AIM_UP),
				luabind::value("INPUT_AIM_DOWN", InputElements::INPUT_AIM_DOWN),
				luabind::value("INPUT_AIM_LEFT", InputElements::INPUT_AIM_LEFT),
				luabind::value("INPUT_AIM_RIGHT", InputElements::INPUT_AIM_RIGHT),
				luabind::value("INPUT_PIEMENU", InputElements::INPUT_PIEMENU),
				luabind::value("INPUT_JUMP", InputElements::INPUT_JUMP),
				luabind::value("INPUT_CROUCH", InputElements::INPUT_CROUCH),
				luabind::value("INPUT_NEXT", InputElements::INPUT_NEXT),
				luabind::value("INPUT_PREV", InputElements::INPUT_PREV),
				luabind::value("INPUT_START", InputElements::INPUT_START),
				luabind::value("INPUT_BACK", InputElements::INPUT_BACK),
				luabind::value("INPUT_COUNT", InputElements::INPUT_COUNT)
			],

			luabind::class_<mouse_buttons>("MouseButtons")
			.enum_("MouseButtons")[
				luabind::value("MOUSE_NONE", MouseButtons::MOUSE_NONE),
				luabind::value("MOUSE_LEFT", MouseButtons::MOUSE_LEFT),
				luabind::value("MOUSE_RIGHT", MouseButtons::MOUSE_RIGHT),
				luabind::value("MOUSE_MIDDLE", MouseButtons::MOUSE_MIDDLE),
				luabind::value("MAX_MOUSE_BUTTONS", MouseButtons::MAX_MOUSE_BUTTONS)
			],

			luabind::class_<joy_buttons>("JoyButtons")
			.enum_("JoyButtons")[
				luabind::value("JOY_NONE", JoyButtons::JOY_NONE),
				luabind::value("JOY_1", JoyButtons::JOY_1),
				luabind::value("JOY_2", JoyButtons::JOY_2),
				luabind::value("JOY_3", JoyButtons::JOY_3),
				luabind::value("JOY_4", JoyButtons::JOY_4),
				luabind::value("JOY_5", JoyButtons::JOY_5),
				luabind::value("JOY_6", JoyButtons::JOY_6),
				luabind::value("JOY_7", JoyButtons::JOY_7),
				luabind::value("JOY_8", JoyButtons::JOY_8),
				luabind::value("JOY_9", JoyButtons::JOY_9),
				luabind::value("JOY_10", JoyButtons::JOY_10),
				luabind::value("JOY_11", JoyButtons::JOY_11),
				luabind::value("JOY_12", JoyButtons::JOY_12),
				luabind::value("MAX_JOY_BUTTONS", JoyButtons::MAX_JOY_BUTTONS)
			],

			luabind::class_<joy_directions>("JoyDirections")
			.enum_("JoyDirections")[
				luabind::value("JOYDIR_ONE", JoyDirections::JOYDIR_ONE),
				luabind::value("JOYDIR_TWO", JoyDirections::JOYDIR_TWO)
			]
		];

		// Assign the manager instances to globals in the lua master state
		luabind::globals(m_MasterState)["TimerMan"] = &g_TimerMan;
		luabind::globals(m_MasterState)["FrameMan"] = &g_FrameMan;
		luabind::globals(m_MasterState)["PostProcessMan"] = &g_PostProcessMan;
		luabind::globals(m_MasterState)["PrimitiveMan"] = &g_PrimitiveMan;
		luabind::globals(m_MasterState)["PresetMan"] = &g_PresetMan;
		luabind::globals(m_MasterState)["AudioMan"] = &g_AudioMan;
		luabind::globals(m_MasterState)["UInputMan"] = &g_UInputMan;
		luabind::globals(m_MasterState)["SceneMan"] = &g_SceneMan;
		luabind::globals(m_MasterState)["ActivityMan"] = &g_ActivityMan;
		luabind::globals(m_MasterState)["MetaMan"] = &g_MetaMan;
		luabind::globals(m_MasterState)["MovableMan"] = &g_MovableMan;
		luabind::globals(m_MasterState)["ConsoleMan"] = &g_ConsoleMan;
		luabind::globals(m_MasterState)["LuaMan"] = &g_LuaMan;
		luabind::globals(m_MasterState)["SettingsMan"] = &g_SettingsMan;

		luaL_dostring(m_MasterState,
			// Override print() in the lua state to output to the console
			"print = function(toPrint) ConsoleMan:PrintString(\"PRINT: \" .. tostring(toPrint)); end;\n"
			// Add cls() as a shortcut to ConsoleMan:Clear()
			"cls = function() ConsoleMan:Clear(); end;"
			// Add package path to the defaults
			"package.path = package.path .. \";Base.rte/?.lua\";\n"
		);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void LuaMan::Destroy() {
		lua_close(m_MasterState);

		for (int i = 0; i < c_MaxOpenFiles; ++i) {
			FileClose(i);
		}
		Clear();
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	int LuaMan::RunScriptedFunction(const std::string &functionName, const std::string &selfObjectName, std::vector<std::string> variablesToSafetyCheck, std::vector<Entity *> functionEntityArguments, std::vector<std::string> functionLiteralArguments) {
		std::string scriptString = "";
		if (!variablesToSafetyCheck.empty()) {
			scriptString += "if ";
			for (const std::string &variableToSafetyCheck : variablesToSafetyCheck) {
				if (&variableToSafetyCheck != &variablesToSafetyCheck[0]) {
					scriptString += " and ";
				}
				scriptString += variableToSafetyCheck;
			}
			scriptString += " then ";
		}
		if (!functionEntityArguments.empty()) {
			scriptString += "local entityArguments = LuaMan.TempEntities; ";
		}
		scriptString += functionName + "(" + selfObjectName;
		if (!functionEntityArguments.empty()) {
			g_LuaMan.SetTempEntityVector(functionEntityArguments);
			for (const Entity *functionEntityArgument : functionEntityArguments) {
				scriptString += ", (To" + functionEntityArgument->GetClassName() + " and To" + functionEntityArgument->GetClassName() + "(entityArguments()) or entityArguments())";
			}
		}
		if (!functionLiteralArguments.empty()) {
			for (const std::string functionLiteralArgument : functionLiteralArguments) {
				scriptString += ", " + functionLiteralArgument;
			}
		}
		scriptString += ");";

		if (!variablesToSafetyCheck.empty()) { scriptString += " end;"; }

		return RunScriptString(scriptString);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	int LuaMan::RunScriptString(const std::string &scriptString, bool consoleErrors) {
		if (scriptString.empty()) {
			return -1;
		}
		int error = 0;

		lua_pushcfunction(m_MasterState, &AddFileAndLineToError);
		try {
			// Load the script string onto the stack and then execute it with pcall. Pcall will call the file and line error handler if there's an error by pointing 2 up the stack to it.
			if (luaL_loadstring(m_MasterState, scriptString.c_str()) || lua_pcall(m_MasterState, 0, LUA_MULTRET, -2)) {
				// Retrieve the error message then pop it off the stack to clean it up
				m_LastError = lua_tostring(m_MasterState, -1);
				lua_pop(m_MasterState, 1);
				if (consoleErrors) {
					g_ConsoleMan.PrintString("ERROR: " + m_LastError);
					ClearErrors();
				}
				error = -1;
			}
		} catch (const std::exception &e) {
			m_LastError = e.what();
			if (consoleErrors) {
				g_ConsoleMan.PrintString("ERROR: " + m_LastError);
				ClearErrors();
			}
			error = -1;
		}

		// Pop the file and line error handler off the stack to clean it up
		lua_pop(m_MasterState, 1);

		return error;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	int LuaMan::RunScriptFile(const std::string &filePath, bool consoleErrors) {
		if (filePath.empty()) {
			m_LastError = "Can't run a script file with an empty filepath!";
			return -1;
		}

		if (!System::PathExistsCaseSensitive(filePath)) {
			m_LastError = "Script file: " + filePath + " doesn't exist!";
			if (consoleErrors) {
				g_ConsoleMan.PrintString("ERROR: " + m_LastError);
				ClearErrors();
			}
			return -1;
		}

		int error = 0;

		lua_pushcfunction(m_MasterState, &AddFileAndLineToError);
		try {
			// Load the script file's contents onto the stack and then execute it with pcall. Pcall will call the file and line error handler if there's an error by pointing 2 up the stack to it.
			if (luaL_loadfile(m_MasterState, filePath.c_str()) || lua_pcall(m_MasterState, 0, LUA_MULTRET, -2)) {
				// Retrieve the error message then pop it off the stack
				m_LastError = lua_tostring(m_MasterState, -1);
				lua_pop(m_MasterState, 1);
				if (consoleErrors) {
					g_ConsoleMan.PrintString("ERROR: " + m_LastError);
					ClearErrors();
				}
				error = -1;
			}
		} catch (const std::exception &e) {
			m_LastError = e.what();
			if (consoleErrors) {
				g_ConsoleMan.PrintString("ERROR: " + m_LastError);
				ClearErrors();
			}
			error = -1;
		}

		// Pop the file and line error handler off the stack to clean it up
		lua_pop(m_MasterState, 1);

		return error;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	bool LuaMan::ExpressionIsTrue(std::string expression, bool consoleErrors) {
		if (expression.empty()) {
			return false;
		}
		bool result = false;

		try {
			// Push the script string onto the stack so we can execute it, and then actually try to run it
			// Assign the result to a dedicated temp global variable
			if (luaL_dostring(m_MasterState, (string("ExpressionResult = ") + expression + string(";")).c_str())) {
				// Retrieve and pop the error message off the stack
				m_LastError = string("When evaluating Lua expression: ") + lua_tostring(m_MasterState, -1);
				lua_pop(m_MasterState, 1);
				if (consoleErrors) {
					g_ConsoleMan.PrintString("ERROR: " + m_LastError);
					ClearErrors();
				}
				return false;
			}
		} catch (const std::exception &e) {
			m_LastError = string("When evaluating Lua expression: ") + e.what();
			if (consoleErrors) {
				g_ConsoleMan.PrintString("ERROR: " + m_LastError);
				ClearErrors();
			}
			return false;
		}
		// Get the result var onto the stack so we can check it
		lua_getglobal(m_MasterState, "ExpressionResult");
		// Now report if it is nil/null or not
		result = lua_toboolean(m_MasterState, -1);
		// Pop the result var so this operation is balanced and leaves the stack as it was
		lua_pop(m_MasterState, 1);

		return result;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	int LuaMan::FileOpen(std::string filename, std::string mode) {
		int fl = -1;

		// Find some suitable file
		for (int i = 0; i < c_MaxOpenFiles; ++i)
			if (m_OpenedFiles.at(i) == 0) {
				fl = i;
				break;
			}

		if (fl == -1) {
			g_ConsoleMan.PrintString("Error: Can't open file, no more slots.");
			return -1;
		}

		//Check for path back-traversing and .rte extension. Everything is allowed to read or write only inside rte's
		std::string dotString = "..";
		std::string rteString = ".rte";

		std::string fullPath = System::GetWorkingDirectory() + filename;

		// Do not open paths with '..'
		if (fullPath.find(dotString) != std::string::npos)
			return -1;

		// Do not open paths that aren't written correctly
		if (!System::PathExistsCaseSensitive(std::filesystem::path(filename).lexically_normal().generic_string()))
			return -1;

		// Allow to edit files only inside .rte folders
		if (fullPath.find(rteString) == std::string::npos)
			return -1;

		// Open file and save handle
		FILE * f = fopen(fullPath.c_str(), mode.c_str());
		if (f) {
			m_OpenedFiles.at(fl) = f;
			return fl;
		}

#ifdef _DEBUG
		g_ConsoleMan.PrintString("Error: Can't open file. " + fullPath);
#endif
		return -1;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void LuaMan::FileClose(int file) {
		if (file > -1 && file < c_MaxOpenFiles && m_OpenedFiles.at(file)) {
			fclose(m_OpenedFiles.at(file));
			m_OpenedFiles.at(file) = nullptr;
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void LuaMan::FileCloseAll() {
		for (int file = 0; file < c_MaxOpenFiles; ++file) {
			FileClose(file);
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	std::string LuaMan::FileReadLine(int file) {
		if (file > -1 && file < c_MaxOpenFiles && m_OpenedFiles.at(file)) {
			char buf[4096];
			fgets(buf, 4095, m_OpenedFiles.at(file));
			return (std::string(buf));
		}
		g_ConsoleMan.PrintString("Error: Tried to read a closed file, or read past EOF.");
		return "";
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void LuaMan::FileWriteLine(int file, std::string line) {
		if (file > -1 && file < c_MaxOpenFiles && m_OpenedFiles.at(file)) {
			fputs(line.c_str(), m_OpenedFiles.at(file));
		} else {
			g_ConsoleMan.PrintString("Error: Tried to write to a closed file.");
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	bool LuaMan::FileEOF(int file) {
		if (file > -1 && file < c_MaxOpenFiles && m_OpenedFiles.at(file) && !feof(m_OpenedFiles.at(file))) {
			return false;
		}
		return true;
	}









/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void LuaMan::ClearUserModuleCache() {
		luaL_dostring(m_MasterState, "for m, n in pairs(package.loaded) do if type(n) == \"boolean\" then package.loaded[m] = nil end end");
	}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void LuaMan::SavePointerAsGlobal(void *objectToSave, const std::string &globalName) {
		// Push the pointer onto the Lua stack
		lua_pushlightuserdata(m_MasterState, objectToSave);
		// Pop and assign that pointer to a global var in the Lua state
		lua_setglobal(m_MasterState, globalName.c_str());
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	bool LuaMan::GlobalIsDefined(std::string globalName) {
		// Get the var you want onto the stack so we can check it
		lua_getglobal(m_MasterState, globalName.c_str());
		// Now report if it is nil/null or not
		bool isDefined = !lua_isnil(m_MasterState, -1);
		// Pop the var so this operation is balanced and leaves the stack as it was
		lua_pop(m_MasterState, 1);

		return isDefined;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	bool LuaMan::TableEntryIsDefined(std::string tableName, std::string indexName) {
		// Push the table onto the stack, checking if it even exists
		lua_getglobal(m_MasterState, tableName.c_str());
		if (!lua_istable(m_MasterState, -1)) {
			//error(m_MasterState, tableName + " is not a table when checking for the " + indexName + " within it.");
			// Clean up and report that there was nothing properly defined here
			lua_pop(m_MasterState, 1);
			return false;
		}

		// Push the value at the requested index onto the stack so we can check if it's anything
		lua_getfield(m_MasterState, -1, indexName.c_str());
		// Now report if it is nil/null or not
		bool isDefined = !lua_isnil(m_MasterState, -1);
		// Pop both the var and the table so this operation is balanced and leaves the stack as it was
		lua_pop(m_MasterState, 2);

		return isDefined;
	}









/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	std::string LuaMan::GetNewPresetID() {
		// Generate the new ID
		char newID[64];
		std::snprintf(newID, sizeof(newID), "Pre%05i", m_NextPresetID);
		// Increment the ID so it will be diff for the next one (improve this primitive approach??)
		m_NextPresetID++;

		return std::string(newID);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	std::string LuaMan::GetNewObjectID() {
		// Generate the new ID
		char newID[64];
		std::snprintf(newID, sizeof(newID), "Obj%05i", m_NextObjectID);
		// Increment the ID so it will be diff for the next one (improve this primitive approach??)
		m_NextObjectID++;

		return std::string(newID);
	}















/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void LuaMan::Update() const {
		lua_gc(m_MasterState, LUA_GCSTEP, 1);
	}
}