#pragma once

#include "assets.hpp"

class Script;

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
    Asset * deserialize( std::istream& in ) override;
    void serialize( const Asset * asset, std::ostream& out ) const override;
};


class MapFactory : public Factory
{
public: 
    //asset 
    Asset * deserialize( std::istream& in ) override;
    void serialize( const Asset * asset, std::ostream& out ) const override;
};

class FontFactory : public Factory
{
public: 
    //asset 
    Asset *  deserialize( std::istream& in ) override;
    void serialize( const Asset * asset, std::ostream& out ) const override;
};

class ScriptFactory : public Factory
{
public:
    //asset 
    Asset * deserialize( std::istream& in ) override;
    void serialize(const  Asset * asset, std::ostream& out ) const override;
};

