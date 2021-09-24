#ifndef _RTESYSTEMINTERFACE_
#define _RTESYSTEMINTERFACE_
#include "RmlUi/Core/SystemInterface.h"

namespace RTE {
	class SystemInterface : public Rml::SystemInterface {
	public:
		SystemInterface();
		~SystemInterface() override;

		/// Get the number of seconds elapsed since the start of the application.
		/// @return Elapsed time, in seconds.
		double GetElapsedTime() override;

		/// <summary> Log the specified message. </summary>
		/// <param name="type"> Type of log message, ERROR, WARNING, etc. </param>
		/// <param name="message"> Message to log. </param>
		/// <return> True to continue execution, false to break into the debugger. </return>
		bool LogMessage(Rml::Log::Type type, const std::string &message) override;

		/// <summary> Set mouse cursor. </summary>
		/// <param name="cursor_name"> Cursor name to activate. </param>
		void SetMouseCursor(const std::string &cursor_name) override;

		/// <summary> Set clipboard text. </summary>
		/// <param name="text"> Text to apply to clipboard. </param>
		void SetClipboardText(const std::string &text) override;

		/// <summary> Get clipboard text. </summary>
		/// <param name="text"> Retrieved text from clipboard. </param>
		void GetClipboardText(std::string &text) override;
	};
} // namespace RTE
#endif
