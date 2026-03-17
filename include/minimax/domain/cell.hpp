#ifndef MINIMAX_DOMAIN_CELL_HPP_2eea79be_7f28_4a2e_a254_b77c4550598e
#define MINIMAX_DOMAIN_CELL_HPP_2eea79be_7f28_4a2e_a254_b77c4550598e

#include "minimax/common/types.hpp"

namespace minimax {
namespace domain {

class Cell {
public:
    explicit Cell(int value);    
    explicit Cell(CellType type, SurpriseBonus bonus = SurpriseBonus::BONUS_20);
    
    ~Cell() = default;
    Cell(const Cell& other) = default;
    Cell& operator=(const Cell& other) = default;
    
    CellType get_type() const;

    int get_value() const;
    SurpriseBonus get_bonus() const;
    
    bool is_consumed() const;
    void consume(Player player);
    
    bool is_selectable() const;

    Player get_consumed_by() const;

private:
    CellType type_;
    int value_;
    SurpriseBonus bonus_;
    bool consumed_;
    Player consumed_by_;
};

}
}

#endif // MINIMAX_DOMAIN_CELL_HPP_2eea79be_7f28_4a2e_a254_b77c4550598e
