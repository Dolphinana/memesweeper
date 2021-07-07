#include "MemeField.h"
#include <assert.h>
#include <random>
#include "SpriteCodex.h"

void MemeField::Tile::Draw(const Vei2 screenPos, Graphics& gfx) const
{
    switch (state)
    {
    case StateTypes::Hidden:
        SpriteCodex::DrawTileButton(screenPos, gfx);
        break;
    case StateTypes::Flagged:
        SpriteCodex::DrawTileButton(screenPos, gfx);
        SpriteCodex::DrawTileFlag(screenPos, gfx);
        break;
    case StateTypes::Revealed:
		SpriteCodex::DrawTile0( screenPos, gfx );
        if (HasMeme())
        {
            SpriteCodex::DrawTileMeme( screenPos, gfx );
        }
        break;
    }
}

void MemeField::Tile::Reveal()
{
    assert(state != StateTypes::Revealed);

    state = StateTypes::Revealed;
}

bool MemeField::Tile::Revealed() const
{
    return state == StateTypes::Revealed;
}

void MemeField::Tile::SpawnMeme()
{
    assert(!HasMeme());
    hasMeme = true;
}
bool MemeField::Tile::HasMeme() const
{
    return hasMeme;
}

RectI MemeField::GetRect() const
{
    return RectI(0, width * SpriteCodex::tileSize, 0, height * SpriteCodex::tileSize);
}

MemeField::Tile& MemeField::TileAt(const Vei2 gridPos)
{
    return fields[gridPos.y * width + gridPos.x];
}

const MemeField::Tile& MemeField::TileAt(const Vei2 gridPos) const
{
    return fields[gridPos.y * width + gridPos.x];
}

MemeField::MemeField(int nMemes)
{
    std::random_device rd;
    std::mt19937 rng( rd() );
    std::uniform_int_distribution<int> xDist(0, width - 1);
    std::uniform_int_distribution<int> yDist(0, height - 1);


    for (int i = 0; i < nMemes; ++i)
    {
        Vei2 newGrid;
        do
        {
            newGrid.x = xDist(rng);
            newGrid.y = yDist(rng);
        } while (TileAt(newGrid).HasMeme());

        TileAt(newGrid).SpawnMeme();
    }
}

void MemeField::Draw(Graphics& gfx) const
{
    gfx.DrawRect(GetRect(), SpriteCodex::baseColor);
    
    for (Vei2 gridPos = { 0,0 }; gridPos.y < height; ++gridPos.y)
    {
        for (gridPos.x = 0; gridPos.x < width; ++gridPos.x)
        {
            TileAt(gridPos).Draw(gridPos * SpriteCodex::tileSize, gfx);
		}
    }
}

void MemeField::OnRevealClick(const Vei2 screenPos)
{
    Vei2 gridPos = ScreenToGrid(screenPos);
    // if NOT REVEALED >:(
    assert(gridPos.x >= 0 && gridPos.x < width&& gridPos.y >= 0 && gridPos.y < height);
    if (!TileAt(gridPos).Revealed())
    {
        TileAt(gridPos).Reveal();
    }
}

Vei2 MemeField::ScreenToGrid(const Vei2 screenPos) const
{
    return screenPos / SpriteCodex::tileSize;
}
