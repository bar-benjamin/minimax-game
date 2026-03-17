#include "minimax/domain/cell.hpp"

namespace minimax {
namespace domain {

Cell::Cell(int value)
: type_(CellType::NUMBER)
, value_(value)
, bonus_(SurpriseBonus::BONUS_20)
, consumed_(false)
, consumed_by_(Player::NONE)
{
}

Cell::Cell(CellType type, SurpriseBonus bonus)
: type_(type)
, value_(0)
, bonus_(bonus)
, consumed_(false)
, consumed_by_(Player::NONE)
{
}

CellType Cell::get_type() const
{   
    return type_;
}

int Cell::get_value() const
{
    return value_;
}

SurpriseBonus Cell::get_bonus() const
{
    return bonus_;
}

bool Cell::is_consumed() const
{
    return consumed_;
}

void Cell::consume(Player player)
{
    consumed_ = true;
    consumed_by_ = player;
}

bool Cell::is_selectable() const
{
    return !consumed_ && type_ != CellType::BLOCKED;
}

Player Cell::get_consumed_by() const
{
    return consumed_by_;
}

}
}
