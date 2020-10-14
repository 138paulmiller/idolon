#pragma once

#include "assets.hpp"


/*
    Factories
*/

#define FACTORY_COUNT 5

class TilesetFactory : public Factory
{
    friend class Sprite;
public: 
    //asset 
    TilesetFactory();
    Asset * deserialize( std::istream& in ) override;
    void serialize( const Asset * asset, std::ostream& out ) const override;
};

/*--------------------------- -------------  MapFactory  ------------------------------------
*/
class MapFactory : public Factory
{
public: 
    //asset 
    MapFactory();
    Asset * deserialize( std::istream& in ) override;
    void serialize( const Asset * asset, std::ostream& out ) const override;
};

/*--------------------------- -------------  FontFactory  ------------------------------------
*/
class FontFactory : public Factory
{
public: 
    //asset 
    FontFactory();
    Asset *  deserialize( std::istream& in ) override;
    void serialize( const Asset * asset, std::ostream& out ) const override;
};

/*--------------------------- -------------  ScriptFactory  ------------------------------------
*/
class ScriptFactory : public Factory
{
public:
    //asset 
    ScriptFactory();
    Asset * deserialize( std::istream& in ) override;
    void serialize(const  Asset * asset, std::ostream& out ) const override;
};



/*--------------------------- -------------  ScriptFactory  ------------------------------------
*/
class GameFactory : public Factory
{
public:
    //asset 
    GameFactory();
    Asset * deserialize( std::istream& in ) override;
    void serialize(const  Asset * asset, std::ostream& out ) const override;
};