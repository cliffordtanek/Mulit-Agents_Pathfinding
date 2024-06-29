#ifndef VECTOR2D_HPP
#define VECTOR2D_HPP

#include <cmath>
#include <iostream>

//! TO DISABLE WARNING FOR NAMELESS STRUCT / UNION
#pragma warning(disable: 4201)

// ========================= //
// Vector2D struct declaration
// ========================= //
typedef union Vector2D
{
	struct { float x, y; };

	float m[2];


	// Constructors
	/*!***********************************************************************
	\brief

		Default ctor
		Initialized an uninitialize Vector2D variable.
	*************************************************************************/
	Vector2D();											// default ctor


	/*!***********************************************************************
	\brief

		Arguments conversion ctor
		Initialized a Vector2D variable with respective function
		call parameters.
	*************************************************************************/
	Vector2D(float _x, float _y);						// arguments ctor


	/*!***********************************************************************
	\brief

		Copy ctor
		Initialized a Vector2D variable with another Vector2D variable.
	*************************************************************************/
	Vector2D(Vector2D const &rhs) = default;			// copy ctor


	/*!***********************************************************************
	\brief

		Copy assignemnt
		Updates a Vector2D variable with another Vector2D variable.
	*************************************************************************/
	Vector2D &operator=(Vector2D const &rhs) = default;	// copy assignment

	Vector2D InwardNormal() const;

	// Assignment operators
	/*!***********************************************************************
	\brief

		Performs addition with right Vector operand and store the result back
		to left Vector operand.

	\param[in] rhs

		Vector to add with.

	\return
		A reference to *this.
	*************************************************************************/
	Vector2D &operator+= (Vector2D const &rhs);


	/*!***********************************************************************
	\brief

		Performs subtraction with right Vector operand and store the result
		back to left Vector operand.

	\param[in] rhs

		Vector to subtract with.

	\return
		A reference to *this.
	*************************************************************************/
	Vector2D &operator-= (Vector2D const &rhs);


	/*!***********************************************************************
	\brief

		Performs multiplication with right Vector operand and store the result
		back to left Vector operand.

	\param[in] rhs

		Vector to multiply with.

	\return
		A reference to *this.
	*************************************************************************/
	Vector2D &operator*= (float rhs);


	/*!***********************************************************************
	\brief

		Performs division with right Vector operand and store the result back
		to left Vector operand.

	\param[in] rhs

		Vector to divide with.

	\return
		A reference to *this.
	*************************************************************************/
	Vector2D &operator/= (float rhs);



	// bool	operator== (Vector2D const& rhs) const;
	/*!***********************************************************************
	\brief

		Returns a Vector2D with the negated variables of left operand Vetor.

	\return
		Vector2D with the negated variables of left operand Vetor.
	*************************************************************************/
	Vector2D operator-() const; 	// Unary operators


	/*!***********************************************************************
	\brief

		This function will return the normalized vector of this.

	\return
			The normalize vector of this.
	*************************************************************************/
	Vector2D Normalize() const;


	/*!***********************************************************************
	\brief

		This function will return the outward normal vector of this.

	\return
			The outward normal.
	*************************************************************************/
	Vector2D OutwardNormal() const;


	/*!***********************************************************************
	\brief

		This function returns the length of the vector.

	\return
			Length of vector.
	*************************************************************************/
	float	Length() const;


	/*!***********************************************************************
	\brief

		This function returns the square of this length.
		(To avoid the square root).


	\return
			Sqaured length of vector.
	*************************************************************************/
	float	SquareLength() const;


	/*!***********************************************************************
	\brief

		This function returns the distance between this and rhs points.

	\param[in] rhs

		2D point.

	\return
			The distance between these 2 2D points.
	*************************************************************************/
	float	Distance(Vector2D const &rhs) const;


	/*!***********************************************************************
	\brief

		In this function,*this and rhs are considered as 2D points.
		The squared distance between these 2 2D points is returned.
		(Avoid the square root).

	\param[in] rhs

		2nd 2D point.

	\return
			The squared distance between these 2 2D points.
	*************************************************************************/
	float	SquareDistance(Vector2D const &rhs) const;


	/*!***********************************************************************
	\brief

		This function returns the dot product between this and rhs.

	\param[in] rhs

		Vector to perform dot product.

	\return
			The dot product between pVec0 and rhs.
	*************************************************************************/
	float	Dot(Vector2D const &rhs) const;


	/*!***********************************************************************
	\brief

		This function returns the cross product magnitude between this and rhs.


	\param[in] rhs

		Vector to perform cross product.

	\return
			The cross product magnitude between this and rhs.
	*************************************************************************/
	float	Cross(Vector2D const &rhs) const;


	/*!***********************************************************************
	\brief

		This function checks if this vector is zero


	\return
			True if the vector is zero. False otherwise
	*************************************************************************/
	bool Zero() const;


} Vector2D, Vec2, point2D, pt2, vec2; // name alias


// ========================================= //
// Vector2D Non-member functions declarations
// ======================================== //

/*!***********************************************************************
\brief

	Returns the result of addition with lhs Vector and rhs Vector.

\param[in] lhs

	Vector to add with.

\param[in] rhs

	Vector to add with.

\return
		Result of addition with lhs Vector and rhs Vector.
*************************************************************************/
Vector2D operator + (Vector2D const &lhs, Vector2D const &rhs);


/*!***********************************************************************
\brief

	Returns the result of subtraction with lhs Vector and rhs Vector.

\param[in] lhs

	Vector to subtract with.

\param[in] rhs

	Vector to subtract with.

\return
		Result of subtraction with lhs Vector and rhs Vector.
*************************************************************************/
Vector2D operator - (Vector2D const &lhs, Vector2D const &rhs);


/*!***********************************************************************
\brief

	Returns the result of multiplication with lhs Vector and rhs float value.

\param[in] lhs

	Vector to multiply with.

\param[in] rhs

	float value to multiply with.

\return
		Result of multiplication with lhs Vector and rhs float value.
*************************************************************************/
Vector2D operator * (Vector2D const &lhs, float rhs);


/*!***********************************************************************
\brief

	Returns the result of multiplication with lhs float value and rhs Vector.

\param[in] lhs

	float value to multiply with.

\param[in] rhs

	Vector to multiply with.

\return
		Result of multiplication with lhs float value and rhs Vector.
*************************************************************************/
Vector2D operator * (float lhs, Vector2D const &rhs);


/*!***********************************************************************
\brief

	Returns the result of division with lhs Vector and rhs float value.

\param[in] lhs

	Vector to divide with.

\param[in] rhs

	float value to divide with.

\return
		Result of division with lhs Vector and rhs float value.
*************************************************************************/
Vector2D operator / (Vector2D const &lhs, float rhs);



/*!***********************************************************************
\brief

	This function overloads the left shift operator to print the vector's
	x & y coordinates to the output console. It is mainly used for
	debugging purposes.

\param[in] os

	std::ostream output stream.

\param[in] rhs

	Vector to print to console.

\return
		The output stream.
*************************************************************************/
std::ostream &operator<<(std::ostream &os, Vector2D const &rhs);


#endif // vector2D_H