#include <functional>
#include <unordered_map>
#include <string>
#include <iostream>
#include "Utility.h"

class Window
{
protected:

	std::string name;
	bool isOpen = false;
	bool canBeOpened = true;

public:

	Window(const std::string &_name = "", bool _canBeOpened = true) : name(_name), canBeOpened(_canBeOpened) { };
	virtual ~Window();

	virtual void onEnter();
	virtual void onUpdate();
	virtual void onExit();

	void open();
	void close();
	bool isWindowOpen();
	bool canWindowBeOpened();
};

class MainMenu : public Window
{
public:

	MainMenu(const std::string &_name = "", bool _canBeOpened = true) : Window(_name, _canBeOpened) { };

	void onEnter() override;
	void onUpdate() override;
	void onExit() override;
};

class Inspector : public Window
{
public:

	Inspector(const std::string &_name = "", bool _canBeOpened = true) : Window(_name, _canBeOpened) { };

	void onEnter() override;
	void onUpdate() override;
	void onExit() override;
};

class MapEditor : public Window
{
public:

	MapEditor(const std::string &_name = "", bool _canBeOpened = true) : Window(_name, _canBeOpened) { };

	void onEnter() override;
	void onUpdate() override;
	void onExit() override;
};

class SaveAsMapPopup : public Window
{
	bool isSaveAsMode = true;
	const char *oldName = nullptr;

public:

	SaveAsMapPopup(const std::string &_name = "", bool _canBeOpened = true) : Window(_name, _canBeOpened) { };

	void onEnter() override;
	void onUpdate() override;
	void onExit() override;

	void initMode(bool _isSaveAsMode, const char *_oldName);
};

class Editor
{
	std::unordered_map<std::string, Window *> windows;

public:

	void init();
	void update();
	void free();
	void openWindow(const std::string &windowName);
	void closeWindow(const std::string &windowName);
	void toggleWindow(const std::string &windowName);
	void createDockspace();
	const std::unordered_map<std::string, Window *> &getWindows();
	void addSpace(int spaceCount);

	template <typename T>
	void addWindow(bool canOpen, bool canBeOpened)
	{
		if constexpr (std::is_base_of_v<Window, std::decay_t<T>>)
		{
			std::string name = typeid(T).name();
			utl::trimString(name, "class ");
			crashIf(windows.count(name), "Window name " + utl::quote(name) + " already exists");

			windows[name] = new T(name, canBeOpened);
			if (canOpen)
				openWindow(name);
			return;
		}

		crashIf(true, utl::quote(typeid(T).name()) + " is not a child of the Window class");
	}

	template <typename T>
	T *getWindow()
	{
		std::string name = typeid(T).name();
		utl::trimString(name, "class ");
		crashIf(!windows.count(name), "Window name " + utl::quote(name) + " does not exist");
		return dynamic_cast<T *>(windows.at(name));
	}
};