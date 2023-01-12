#ifndef BLACKBODYTABLE_HPP
#define BLACKBODYTABLE_HPP

#include <vector>
#include <SFML/Graphics/Color.hpp>

class BlackBodyTable {
public:
    BlackBodyTable();
    inline sf::Color getColor(float temperature) const;

private:
    std::vector<sf::Color> _table;
    std::size_t _offset;
};


sf::Color BlackBodyTable::getColor(float temperature) const {
    std::size_t i{std::clamp(static_cast<std::size_t>(temperature), _offset, _table.size() - 1 + _offset)};
    return _table[i - _offset];
}

#endif // BLACKBODYTABLE_HPP
