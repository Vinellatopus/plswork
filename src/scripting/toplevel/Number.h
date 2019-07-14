/**************************************************************************
    Lightspark, a free flash player implementation

    Copyright (C) 2009-2013  Alessandro Pignotti (a.pignotti@sssup.it)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
**************************************************************************/

#ifndef SCRIPTING_TOPLEVEL_NUMBER_H
#define SCRIPTING_TOPLEVEL_NUMBER_H 1

#include <cmath>
#include "compat.h"
#include "asobject.h"

namespace lightspark
{

class Number : public ASObject
{
friend ASObject* abstract_d(SystemState* sys,number_t i);
friend ASObject* abstract_d_constant(SystemState* sys,number_t i);
friend ASObject* abstract_di(SystemState* sys,int32_t i);
friend class ABCContext;
friend class ABCVm;
private:
	static void purgeTrailingZeroes(char* buf);
	static tiny_string purgeExponentLeadingZeros(const tiny_string& exponentialForm);
	static int32_t countSignificantDigits(double v);
public:
	Number(Class_base* c, double v=Number::NaN):ASObject(c,T_NUMBER),dval(v),isfloat(true){}
	static const number_t NaN;
	union {
		number_t dval;
		int64_t ival;
	};
	bool isfloat:1;
	inline void setNumber(number_t v)
	{
		isfloat = true;
		dval=v;
	}
	inline number_t toNumber() { return isfloat ? dval : ival; }
	inline bool destruct() { dval=Number::NaN; isfloat = true; return destructIntern(); }
	ASFUNCTION_ATOM(_constructor);
	ASFUNCTION_ATOM(_toString);
	ASFUNCTION_ATOM(_toLocaleString);
	ASFUNCTION_ATOM(toExponential);
	ASFUNCTION_ATOM(toPrecision);
	ASFUNCTION_ATOM(toFixed);
	ASFUNCTION_ATOM(_valueOf);
	tiny_string toString();
	static tiny_string toString(number_t val);
	static tiny_string toStringRadix(number_t val, int radix);
	static tiny_string toExponentialString(double v, int32_t fractionDigits);
	static tiny_string toFixedString(double v, int32_t fractionDigits);
	static tiny_string toPrecisionString(double v, int32_t precision);
	static bool isInteger(number_t val)
	{
		return trunc(val) == val;
	}
	unsigned int toUInt()
	{
		return isfloat ? (unsigned int)(dval) : ival;
	}
	int64_t toInt64()
	{
		if (!isfloat) return ival;
		if(std::isnan(dval) || std::isinf(dval))
			return INT64_MAX;
		return (int64_t)dval;
	}

	/* ECMA-262 9.5 ToInt32 */
	int32_t toInt()
	{
		if (!isfloat) return ival;
		return toInt(dval);
	}
	static int32_t toInt(number_t val)
	{
		double posInt;

		/* step 2 */
		if(std::isnan(val) || std::isinf(val) || val == 0.0)
			return 0;
		/* step 3 */
		posInt = floor(fabs(val));
		/* step 4 */
		if (posInt > 4294967295.0)
			posInt = fmod(posInt, 4294967296.0);
		/* step 5 */
		if (posInt >= 2147483648.0) {
			// follow tamarin
			if(val < 0.0)
				return 0x80000000 - (int32_t)(posInt - 2147483648.0);
			else
				return 0x80000000 + (int32_t)(posInt - 2147483648.0);
		}
		return (int32_t)(val < 0.0 ? -posInt : posInt);
	}
	TRISTATE isLess(ASObject* o);
	bool isEqual(ASObject* o);
	static void buildTraits(ASObject* o){}
	static void sinit(Class_base* c);
	ASFUNCTION_ATOM(generator);
	std::string toDebugString();
	//Serialization interface
	void serialize(ByteArray* out, std::map<tiny_string, uint32_t>& stringMap,
				std::map<const ASObject*, uint32_t>& objMap,
				std::map<const Class_base*, uint32_t>& traitsMap);
};


};

#endif /* SCRIPTING_TOPLEVEL_NUMBER_H */
