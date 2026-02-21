#pragma once
#include <iostream>
#include <map>
#include <string>
#include <SFML/Graphics.hpp>

struct LibraryElement
{
    std::wstring name;
    sf::Texture texture;
};

struct FieldElement
{
    int libraryIndex;
    sf::Vector2f position;
};

class GameModel
{
private:
    std::vector<LibraryElement> m_library;
    std::vector<FieldElement> m_placedElements;

    std::map<std::pair<int, int>, std::vector<int> > m_recipes;
    std::unordered_map<std::wstring, int> m_nameToIndex;

public:
    GameModel() = default;

    void InitLibrary()
    {
        std::vector<std::pair<std::wstring, std::string> > elementData = {
            {L"Земля", "earth.png"},
            {L"Огонь", "fire.png"},
            {L"Вода", "water.png"},
            {L"Воздух", "air.png"},
            {L"Лава", "lava.jpg"},
            {L"Пар", "steam.png"},
            {L"Пыль", "dust.jpg"},
            {L"Порох", "gunpowder.jpg"},
            {L"Взрыв", "explosion.jpg"},
            {L"Дым", "smoke.jpg"},
            {L"Энергия", "energy.jpg"},
            {L"Камень", "rock.jpg"},
            {L"Металл", "metall.png"},
            {L"Электричество", "electricity.png"},
            {L"Водород", "hydrogen.png"},
            {L"Кислород", "oxygen.png"},
            {L"Озон", "ozon.png"},
            {L"Грязь", "dirty.png"},
            {L"Гейзер", "geyser.png"},
            {L"Паровой котел", "steam-boiler.png"},
            {L"Давление", "preasure.png"},
            {L"Вулкан", "vulcano.png"},
            {L"Гремучий газ", "explosive-gas.png"},
            {L"Болото", "boloto.jpg"},
            {L"Спирт", "ethyl-alcohol.png"},
            {L"Коктейль Молотова", "molotov.png"},
            {L"Жизнь", "life.png"},
            {L"Бактерии", "bacteria.png"},
            {L"Водка", "vodka.png"},
            {L"Буря", "tornado.jpg"},
        };

        m_library.clear();
        m_nameToIndex.clear();

        for (size_t i = 0; i < elementData.size(); ++i)
        {
            const auto& [name, filename] = elementData[i];
            LibraryElement elem;
            elem.name = name;

            std::string path = "D:/Projects/6-SEM/CG/2-lab/task3/Resources/" + filename;
            if (!elem.texture.loadFromFile(path))
            {
                std::cerr << "Failed to load texture " << path
                        << " for " << std::string(name.begin(), name.end()) << std::endl;
                sf::Image image = sf::Image({80, 80}, sf::Color::Blue);
                if (!elem.texture.loadFromImage(image)) {}
            }
            m_library.push_back(elem);
            m_nameToIndex[name] = static_cast<int>(i);
        }

        InitRecipes();
    }

    void InitRecipes()
    {
        auto idx = [this](const std::wstring& name) -> int
        {
            auto it = m_nameToIndex.find(name);
            if (it != m_nameToIndex.end()) return it->second;
            std::wcerr << L"Recipe error: element " << name << L" not found" << std::endl;
            return -1;
        };

        // Огонь + Вода = Пар, Спирт
        m_recipes[{idx(L"Огонь"), idx(L"Вода")}] = {idx(L"Пар"), idx(L"Спирт")};
        // Огонь + Земля = Лава
        m_recipes[{idx(L"Огонь"), idx(L"Земля")}] = {idx(L"Лава")};
        // Воздух + Земля = Пыль
        m_recipes[{idx(L"Воздух"), idx(L"Земля")}] = {idx(L"Пыль")};
        // Огонь + Пыль = Порох
        m_recipes[{idx(L"Огонь"), idx(L"Пыль")}] = {idx(L"Порох")};
        // Порох + Огонь = Взрыв, Дым
        m_recipes[{idx(L"Порох"), idx(L"Огонь")}] = {idx(L"Взрыв"), idx(L"Дым")};
        // Воздух + Огонь = Энергия
        m_recipes[{idx(L"Воздух"), idx(L"Огонь")}] = {idx(L"Энергия")};
        // Лава + Вода = Пар, Камень
        m_recipes[{idx(L"Лава"), idx(L"Вода")}] = {idx(L"Пар"), idx(L"Камень")};
        // Воздух + Энергия = Буря
        m_recipes[{idx(L"Воздух"), idx(L"Энергия")}] = {idx(L"Буря")};
        // Огонь + Камень = Металл
        m_recipes[{idx(L"Огонь"), idx(L"Камень")}] = {idx(L"Металл")};
        // Металл + Энергия = Электричество
        m_recipes[{idx(L"Металл"), idx(L"Энергия")}] = {idx(L"Электричество")};
        // Электричество + Вода = Водород, Кислород
        m_recipes[{idx(L"Электричество"), idx(L"Вода")}] = {idx(L"Водород"), idx(L"Кислород")};
        // Электричество + Кислород = Озон
        m_recipes[{idx(L"Электричество"), idx(L"Кислород")}] = {idx(L"Озон")};
        // Пыль + Вода = Грязь
        m_recipes[{idx(L"Пыль"), idx(L"Вода")}] = {idx(L"Грязь")};
        // Пар + Земля = Гейзер
        m_recipes[{idx(L"Пар"), idx(L"Земля")}] = {idx(L"Гейзер")};
        // Пар + Металл = Паровой котел
        m_recipes[{idx(L"Пар"), idx(L"Металл")}] = {idx(L"Паровой котел")};
        // Паровой котел + Пар = Давление
        m_recipes[{idx(L"Паровой котел"), idx(L"Пар")}] = {idx(L"Давление")};
        // Лава + Давление = Вулкан
        m_recipes[{idx(L"Лава"), idx(L"Давление")}] = {idx(L"Вулкан")};
        // Водород + Кислород = Гремучий газ
        m_recipes[{idx(L"Водород"), idx(L"Кислород")}] = {idx(L"Гремучий газ")};
        // Вода + Земля = Болото
        m_recipes[{idx(L"Вода"), idx(L"Земля")}] = {idx(L"Болото")};
        // Спирт + Огонь = Коктейль Молотова
        m_recipes[{idx(L"Спирт"), idx(L"Огонь")}] = {idx(L"Коктейль Молотова")};
        // Болото + Энергия = Жизнь
        m_recipes[{idx(L"Болото"), idx(L"Энергия")}] = {idx(L"Жизнь")};
        // Жизнь + Болото = Бактерии
        m_recipes[{idx(L"Жизнь"), idx(L"Болото")}] = {idx(L"Бактерии")};
        // Спирт + Вода = Водка
        m_recipes[{idx(L"Спирт"), idx(L"Вода")}] = {idx(L"Водка")};
    }

    bool GetCombinationResult(int idxA, int idxB, std::vector<int>& outResults) const
    {
        auto it = m_recipes.find({idxA, idxB});
        if (it != m_recipes.end())
        {
            outResults = it->second;
            return true;
        }
        it = m_recipes.find({idxB, idxA});
        if (it != m_recipes.end())
        {
            outResults = it->second;
            return true;
        }
        return false;
    }

    void AddFieldElement(int libIndex, sf::Vector2f pos)
    {
        m_placedElements.push_back({libIndex, pos});
    }

    void RemoveFieldElement(int index)
    {
        if (index >= 0 && index < m_placedElements.size())
            m_placedElements.erase(m_placedElements.begin() + index);
    }

    void UpdateFieldElementPosition(int index, sf::Vector2f pos)
    {
        if (index < 0 || index >= m_placedElements.size()) return;
        FieldElement elem = m_placedElements[index];
        elem.position = pos;
        m_placedElements.erase(m_placedElements.begin() + index);
        m_placedElements.push_back(elem);
    }

    const std::vector<LibraryElement>& GetLibrary() const
    {
        return m_library;
    }

    const std::vector<FieldElement>& GetFieldElements() const
    {
        return m_placedElements;
    }
};
