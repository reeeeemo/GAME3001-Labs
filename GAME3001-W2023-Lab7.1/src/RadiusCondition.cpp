#include "RadiusCondition.h"

RadiusCondition::RadiusCondition(const bool within_radiu)
{
	m_name = "Close Combat Condition";
	SetIsWithinRadius(within_radiu);
}

RadiusCondition::~RadiusCondition()
= default;

void RadiusCondition::SetIsWithinRadius(const bool state)
{
	m_isWithinRadius = state;
}

bool RadiusCondition::Condition()
{
	return m_isWithinRadius;
}
