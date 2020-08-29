#pragma once

#include "assets.hpp"

class Script;

namespace Game
{
    class Desc;
}
namespace Graphics
{
    class Tileset;
    class Map;
    class Font;
}

class TilesetFactory : public Factory
{
    friend class Sprite;
public: 
    //asset 
    TilesetFactory();
    Asset * deserialize( std::istream& in ) override;
    void serialize( const Asset * asset, std::ostream& out ) const override;
};


class MapFactory : public Factory
{
public: 
    //asset 
    MapFactory();
    Asset * deserialize( std::istream& in ) override;
    void serialize( const Asset * asset, std::ostream& out ) const override;
};

class FontFactory : public Factory
{
public: 
    //asset 
    FontFactory();
    Asset *  deserialize( std::istream& in ) override;
    void serialize( const Asset * asset, std::ostream& out ) const override;
};

class ScriptFactory : public Factory
{
public:
    //asset 
    ScriptFactory();
    Asset * deserialize( std::istream& in ) override;
    void serialize(const  Asset * asset, std::ostream& out ) const override;
};


class GameDescFactory : public Factory
{
public:
    //asset 
    GameDescFactory();
    Asset * deserialize( std::istream& in ) override;
    void serialize(const  Asset * asset, std::ostream& out ) const override;
};


