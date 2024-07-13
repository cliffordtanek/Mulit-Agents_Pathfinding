#pragma once

#include "Vector2D.h"
#include "Debug.h"
#include "imgui.h"
#include <sstream>
#include <iomanip>
#include "MathLib.h"
#include <SFML/Graphics.hpp>

using namespace std::string_literals;

extern sf::View minimap;
extern sf::View view;
extern sf::RenderWindow window;

// constants
//#define EPSILON 0.000001f
//#define PI 3.14159f
#define EULER 2.718282f
#define SQRT2 1.414214f
#define CAM_MOVE 750.f * dt
#define CAM_ZOOM 7.5f * dt
#define INVALID -1

// sizes
#define LEN 16 // maximum length of some string (for printing to look neat)
#define LONG_LEN 32
#define SMALL 64
#define MEDIUM 256
#define BIG 1024

// printing
#define nl "\n"
#define PRINT(...) utl::print(#__VA_ARGS__, __VA_ARGS__)

// misc
#define WHITESPACE " \n\t\r\v\f"
#define UNREFERENCED(param) param
#define crashIf(condition, reason) do { if (condition) dbg::log(reason, __FILE__, __LINE__); } while(0)
#define ALIVE(type, entity) factory.isEntityAlive<type>(entity)

/*! ------------ ImGui Colors ------------ */

#define BLACK IM_COL32(0, 0, 0, 255)
#define WHITE IM_COL32(255, 255, 255, 255)
#define RED IM_COL32(130, 11, 11, 255)
#define GREEN IM_COL32(9, 188, 14, 255)
#define BLUE IM_COL32(26, 128, 204, 255)
#define OLIVE IM_COL32(150, 200, 100, 255)
#define ORANGE IM_COL32(230, 153, 102, 255)

#define LIGHT_ROSE IM_COL32(0xFF, 0x99, 0xC8, 255)
#define LIGHT_YELLOW IM_COL32(0xFC, 0xF6, 0xBD, 255)
#define LIGHT_GREEN IM_COL32(0xD0, 0xF4, 0xDE, 255)
#define LIGHT_BLUE IM_COL32(0xA9, 0xDE, 0xF9, 255)
#define LIGHT_PURPLE IM_COL32(0xE4, 0xC1, 0xF9, 255)

#define MED_GREEN IM_COL32(19, 150, 12, 255)

#define DARK_GREEN IM_COL32(33, 103, 7, 255)
#define DARK_BLUE IM_COL32(21, 34, 56, 255)

#define TRANS IM_COL32(0, 0, 0, 0)
#define TRANS_BLACK IM_COL32(0, 0, 0, 128)
#define TRANS_WHITE IM_COL32(255, 255, 255, 128)
#define FADE_WHITE IM_COL32(255, 255, 255, 32)
#define TRANS_OLIVE IM_COL32(150, 200, 100, 128)

namespace utl
{
	/*! ------------ Math ------------ */

	// @brief checks if two floats can be considered equal
	// @param lhs: the first float
	// @param rhs: the second float
	// @return whether they can be considered equal
	inline bool isEqual(float lhs, float rhs)
	{
		return fabs(lhs - rhs) < EPSILON;
	}

	// @brief converts the given angle from radians to degrees
	// @param radians
	// @return degrees
	inline float radToDeg(float rad)
	{
		return rad * 180.f / static_cast<float>(PI);
	}

	// @brief converts the given angle from degrees to radians
	// @param degrees
	// @param radians
	inline float degToRad(float deg)
	{
		return deg * static_cast<float>(PI) / 180.f;
	}

	inline float calcRot(Vec2 dir)
	{
		return std::atan2f(dir.y, dir.x);
	}

	// @brief limits the given value within the given boundaries
	// @param value: the number to check
	// @param upper: the upper boundary
	// @param lower: the lower boundary
	// @return the original number if it's within the boundaries, else the boundary that it crosses
	inline float clamp(float value, float lower, float upper)
	{
		return value > upper ? upper : value < lower ? lower : value;
	}

	// @brief rotates a 2D point around a centre in radians
	// @param pos: the position of the point when angle is 0
	// @param angle: the angle to rotate the point by
	// @param rad: the distance between the centre and the point
	// @param centre: the position of the point to rotate around
	inline Vec2 rotate(Vec2 pos, float angle, float rad, Vec2 centre)
	{
		Vec2 dir = pos - centre, temp;
		dir /= rad;
		temp.x = cosf(angle) * dir.x - sinf(angle) * dir.y;
		temp.y = sinf(angle) * dir.x + cosf(angle) * dir.y;
		temp *= rad;
		temp += centre;
		return temp;
	}

	// @brief normalises a number
	// @param val: the number to normalise
	// @returns 1.f if the number is positive, -1.f if it's negative, and 0.f otherwise
	template <typename T>
	T normalise(T val)
	{
		return val ? val / std::abs(val) : T();
	}

	/*! ------------ Strings ------------ */

	// Helper function to split variable names
	inline std::vector<std::string> splitString(const std::string &toSplit, char delim = ' ')
	{
		std::string word;
		std::istringstream line(toSplit);
		std::vector<std::string> words;

		while (std::getline(line, word, delim))
			words.push_back(word);
		return words;
	}

	// @brief removes the leading and trailing whitespaces of the given string
	// @brief toTrim: the string to trim (this value is modified to be the same as the return value too)
	// @returns the trimmed string
	inline std::string trimString(std::string &toTrim)
	{
		size_t pos = toTrim.find_first_not_of(WHITESPACE);
		if (pos != std::string::npos)
			toTrim = toTrim.substr(pos);
		pos = toTrim.find_last_not_of(WHITESPACE);
		if (pos != std::string::npos)
			toTrim = toTrim.substr(0, pos + 1);
		return toTrim;
	}

	// warning: no error checking
	inline std::string trimString(std::string &toTrim, const std::string &toFind, bool isAtFront = true)
	{
		if (isAtFront)
			toTrim = toTrim.substr(toTrim.find(toFind) + toFind.length());
		else
			toTrim = toTrim.substr(0, toTrim.rfind(toFind));
		return toTrim;
	}

	template <typename T>
	std::string ptrToStr(T *ptr)
	{
		std::stringstream ss;
		ss << static_cast<const void *>(ptr);
		return ss.str();
	}

	/*! ------------ Printing ------------ */

	// @brief adds quotes to the given string
	// @param delim: specify something else to add instead of quotes at the start and end of the string
	// @return a string concatenated with quotes
	inline std::string quote(const std::string &str, char delim = '"')
	{
		std::ostringstream oss;
		oss << std::quoted(str, delim);
		return oss.str();
	}

	// @brief retrieves the function signature of the given function
	// @return a string representing the function signature
	template <typename Func>
	std::string getFuncName(Func const &func)
	{
		UNREFERENCED(func);
		return typeid(typename std::remove_pointer<typename std::decay<Func>::type>::type).name();
	}

	// Function to print variable names and values
	template<typename T>
	void printVariable(const std::string &name, const T &value)
	{
		std::cout << name << ": " << value;
	}

	// Base case for recursion
	inline void printVariables(const std::vector<std::string> &, size_t)
	{
		std::cout << nl;
	}

	// Function to print each variable with its name and value
	template<typename T, typename... Args>
	void printVariables(const std::vector<std::string> &names, size_t index, 
		const T &value, const Args&... args) 
	{
		if (index < names.size())
		{
			utl::printVariable(names[index], value);
			if (index < names.size() - 1)
				std::cout << ',';
			utl::printVariables(names, index + 1, args...);  // Recursive call to print next variable
		}
	}

	// Main function to print the variables
	template<typename... Args>
	void print(const std::string &namesStr, const Args&... args)
	{
		std::vector<std::string> names = utl::splitString(namesStr, ',');
		utl::printVariables(names, 0, args...);
	}

	/*! ------------ Random ------------ */

	// @brief returns a random int between the given minimum and maximum
	// @brief min: the minimum number, inclusive
	// @brief max: the maximum number, inclusive
	// @return a number between the minimum and maximum
	inline int randInt(int min, int max)
	{
		return rand() % (max - min + 1) + min;
	}

	// @brief returns a random float between the given minimum and maximum
	// @brief min: the minimum number, inclusive
	// @brief max: the maximum number, inclusive
	// @return a number between the minimum and maximum
	inline float randFloat(float min, float max)
	{
		return static_cast<float>(rand()) / RAND_MAX * (max - min) + min;
	}

	/*! ------------ Lerps ------------ */

	// @brief Calculates the percentage between two values
	// @param start The starting value
	// @param end The ending value
	// @param curr The current value
	// @return The percentage between the start and end values represented by curr
	inline float getPercentage(float start, float end, float curr)
	{
		return (curr - start) / (end - start);
	}


	// @brief Performs linear interpolation between two values
	// @param start The starting value
	// @param end The ending value
	// @param percentage The interpolation percentage between start and end
	// @return The interpolated value
	inline float lerp(float start, float end, float percentage)
	{
		return start + (end - start) * percentage;
	}

	// @brief Applies ease-in function to the current value
	// @param curr The current value
	// @param deg The degree of ease-in
	// @return The eased-in value
	inline float easeIn(float curr, float deg = 2.f)
	{
		return powf(curr, deg);
	}

	// @brief Flips the current value
	// @param curr The current value
	// @return The flipped value
	inline float flip(float curr)
	{
		return 1 - curr;
	}

	// @brief Applies ease-out function to the current value
	// @param curr The current value
	// @param deg The degree of ease-out
	// @return The eased-out value
	inline float easeOut(float curr, float deg = 2.f)
	{
		return flip(powf(flip(curr), deg));
	}

	// @brief Applies ease-in-out function to the current value
	// @param curr The current value
	// @param degIn The degree of ease-in
	// @param degOut The degree of ease-out
	// @return The eased-in-out value
	inline float easeInOut(float curr, float degIn = 2.f, float degOut = 2.f)
	{
		return lerp(easeIn(curr, degIn), easeOut(curr, degOut), curr);
	}

	/*! ------------ ImGui Callbacks ------------ */

	// @brief a function that will be called by ImGui::InputText() to prevent the user from typing non-alphanumeric characters
	// @brief data: the data sent from ImGui::InputText()
	// @return true if the user types a character that is not allowed, false otherwise
	inline int filterAlphanum(ImGuiInputTextCallbackData *data)
	{
		// Allow only alphanumeric characters, underscores, and spaces (does not work with special chars like backspace)
		if (isalnum(data->EventChar) || data->EventChar == '_' || data->EventChar == ' ')
			return 0; // Accept the character

		return 1; // Reject the character
	}
}

/*! ------------ ImGui Operator Overloads (different from those in Vec2) ------------ */

inline ImVec2 operator+(ImVec2 vec1, ImVec2 vec2)
{
	return { vec1.x + vec2.x, vec1.y + vec2.y };
}

inline ImVec2 operator-(ImVec2 vec1, ImVec2 vec2)
{
	return { vec1.x - vec2.x, vec1.y - vec2.y };
}

inline ImVec2 operator*(ImVec2 vec1, ImVec2 vec2)
{
	return { vec1.x * vec2.x, vec1.y * vec2.y };
}

inline ImVec2 operator/(ImVec2 vec1, ImVec2 vec2)
{
	return { vec1.x / vec2.x, vec1.y / vec2.y };
}

inline ImVec2 operator/(ImVec2 vec1, std::pair<int, int> pair)
{
	return { vec1.x / pair.first, vec1.y / pair.second };
}

inline bool operator>(ImVec2 vec1, ImVec2 vec2)
{
	return vec1.x > vec2.x && vec1.y > vec2.y;
}

inline ImVec2 operator+(ImVec2 vec, float flt)
{
	return { vec.x + flt, vec.y + flt };
}

inline ImVec2 operator-(ImVec2 vec, float flt)
{
	return { vec.x - flt, vec.y - flt };
}

inline ImVec2 operator*(ImVec2 vec, float flt)
{
	return { vec.x * flt, vec.y * flt };
}

inline ImVec2 operator/(ImVec2 vec, float flt)
{
	return { vec.x / flt, vec.y / flt };
}

// return vector with both values equal to the larger axis value
inline ImVec2 operator++(ImVec2 vec)
{
	float larger = std::max(vec.x, vec.y);
	return { larger, larger };
}

// return vector with both values equal to the smaller axis value
inline ImVec2 operator--(ImVec2 vec)
{
	float smaller = std::min(vec.x, vec.y);
	return { smaller, smaller };
}