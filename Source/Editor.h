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

public:

	Window(const std::string windowName = "") : name(windowName) { };
	virtual ~Window();

	virtual void onEnter();
	virtual void onUpdate();
	virtual void onExit();

	void open();
	void close();
	bool isWindowOpen();
};

class MainMenu : public Window
{
public:

	MainMenu(const std::string windowName = "") : Window(windowName) { }

	void onEnter() override;
	void onUpdate() override;
	void onExit() override;
};

class Inspector : public Window
{
public:

	Inspector(const std::string windowName = "") : Window(windowName) { }

	void onEnter() override;
	void onUpdate() override;
	void onExit() override;
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
	void addWindow()
	{
		if constexpr (std::is_base_of_v<Window, std::decay_t<T>>)
		{
			std::string name = typeid(T).name();
			utl::trimString(name, "class ");
			crashIf(windows.count(name), "Window name " + utl::quote(name) + " already exists");

			windows[name] = new T(name);
			openWindow(name);
			return;
		}

		crashIf(true, utl::quote(typeid(T).name()) + " is not a child of the Window class");
	}
};