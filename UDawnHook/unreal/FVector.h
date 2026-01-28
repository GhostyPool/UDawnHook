#pragma once

struct FVector
{
	double X;
	double Y;
	double Z;

	FVector() = default;
	FVector(double x, double y, double z)
		: X(x), Y(y), Z(z)
	{
	}

	FVector operator+(const FVector& other) const
	{
		return FVector(X + other.X, Y + other.Y, Z + other.Z);
	}

	FVector operator-(const FVector& other) const
	{
		return FVector(X - other.X, Y - other.Y, Z - other.Z);
	}

	FVector operator*(const FVector& other) const
	{
		return FVector(X * other.X, Y * other.Y, Z * other.Z);
	}

	FVector operator*(const float& f) const
	{
		return FVector(X * f, Y * f, Z * f);
	}

	FVector operator/(const FVector& other) const
	{
		return FVector(X / other.X, Y / other.Y, Z / other.Z);
	}

	bool operator==(const FVector& other) const
	{
		return X == other.X && Y == other.Y && Z == other.Z;
	}

	bool operator!=(const FVector& other) const
	{
		return X != other.X && Y != other.Y && Z != other.Z;
	}

	FVector operator-() const
	{
		return FVector(-X, -Y, -Z);
	}

	FVector& operator+=(const FVector& other)
	{
		X += other.X;
		Y += other.Y;
		Z += other.Z;

		return *this;
	}

	FVector& operator-=(const FVector& other)
	{
		X -= other.X;
		Y -= other.Y;
		Z -= other.Z;

		return *this;
	}

	FVector& operator*=(const FVector& other)
	{
		X *= other.X;
		Y *= other.Y;
		Z *= other.Z;

		return *this;
	}

	FVector& operator*=(const float &f)
	{
		X *= f;
		Y *= f;
		Z *= f;

		return *this;
	}

	FVector& operator/=(const FVector& other)
	{
		X /= other.X;
		Y /= other.Y;
		Z /= other.Z;

		return *this;
	}
};