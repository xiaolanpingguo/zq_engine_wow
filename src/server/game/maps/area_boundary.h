#pragma once


#include "../entities/position.h"


namespace zq{

class  AreaBoundary
{
public:
	bool IsWithinBoundary(Position const* pos) const { return pos && (IsWithinBoundaryArea(pos) != _isInvertedBoundary); }
	bool IsWithinBoundary(Position const& pos) const { return IsWithinBoundary(&pos); }

	virtual ~AreaBoundary() { }

protected:
	explicit AreaBoundary(bool isInverted) : _isInvertedBoundary(isInverted) { }

	struct DoublePosition : Position
	{
		DoublePosition(double x = 0.0, double y = 0.0, double z = 0.0, float o = 0.0f)
			: Position(float(x), float(y), float(z), o), DoublePosX(x), DoublePosY(y), DoublePosZ(z) { }

		DoublePosition(float x, float y = 0.0f, float z = 0.0f, float o = 0.0f)
			: Position(x, y, z, o), DoublePosX(x), DoublePosY(y), DoublePosZ(z) { }

		DoublePosition(Position const& pos)
			: Position(pos), DoublePosX(pos.m_positionX), DoublePosY(pos.m_positionY), DoublePosZ(pos.m_positionZ) { }

		double GetDoublePositionX() const { return DoublePosX; }
		double GetDoublePositionY() const { return DoublePosY; }
		double GetDoublePositionZ() const { return DoublePosZ; }

		double GetDoubleExactDist2dSq(DoublePosition const& pos) const
		{
			double const offX = GetDoublePositionX() - pos.GetDoublePositionX();
			double const offY = GetDoublePositionY() - pos.GetDoublePositionY();
			return (offX * offX) + (offY * offY);
		}

		Position* sync()
		{
			m_positionX = float(DoublePosX);
			m_positionY = float(DoublePosY);
			m_positionZ = float(DoublePosZ);
			return this;
		}

		double DoublePosX;
		double DoublePosY;
		double DoublePosZ;
	};

	virtual bool IsWithinBoundaryArea(Position const* pos) const = 0;

private:
	bool _isInvertedBoundary;
};

class  RectangleBoundary : public AreaBoundary
{
public:
    // X axis is north/south, Y axis is east/west, larger values are northwest
    RectangleBoundary(float southX, float northX, float eastY, float westY, bool isInverted = false);

protected:
    bool IsWithinBoundaryArea(Position const* pos) const override;

private:
    float const _minX, _maxX, _minY, _maxY;
};

class  CircleBoundary : public AreaBoundary
{
public:
    CircleBoundary(Position const& center, double radius, bool isInverted = false);
    CircleBoundary(Position const& center, Position const& pointOnCircle, bool isInverted = false);

protected:
    bool IsWithinBoundaryArea(Position const* pos) const override;

private:
    DoublePosition const _center;
    double const _radiusSq;
};

class  EllipseBoundary : public AreaBoundary
{
public:
    EllipseBoundary(Position const& center, double radiusX, double radiusY, bool isInverted = false);

protected:
    bool IsWithinBoundaryArea(Position const* pos) const override;

private:
    DoublePosition const _center;
    double const _radiusYSq, _scaleXSq;
};

class  TriangleBoundary : public AreaBoundary
{
public:
    TriangleBoundary(Position const& pointA, Position const& pointB, Position const& pointC, bool isInverted = false);

protected:
    bool IsWithinBoundaryArea(Position const* pos) const override;

private:
    DoublePosition const _a, _b, _c;
    double const _abx, _bcx, _cax, _aby, _bcy, _cay;
};

class  ParallelogramBoundary : public AreaBoundary
{
public:
    // Note: AB must be orthogonal to AD
    ParallelogramBoundary(Position const& cornerA, Position const& cornerB, Position const& cornerD, bool isInverted = false);

protected:
    bool IsWithinBoundaryArea(Position const* pos) const override;

private:
    DoublePosition const _a, _b, _d, _c;
    double const _abx, _dax, _aby, _day;
};

class  ZRangeBoundary : public AreaBoundary
{
public:
    ZRangeBoundary(float minZ, float maxZ, bool isInverted = false);

protected:
    bool IsWithinBoundaryArea(Position const* pos) const override;

private:
    float const _minZ, _maxZ;
};

class  BoundaryUnionBoundary : public AreaBoundary
{
public:
    BoundaryUnionBoundary(AreaBoundary const* b1, AreaBoundary const* b2, bool isInverted = false);

protected:
    virtual ~BoundaryUnionBoundary();
    bool IsWithinBoundaryArea(Position const* pos) const override;

private:
    AreaBoundary const* const _b1;
    AreaBoundary const* const _b2;
};

}
