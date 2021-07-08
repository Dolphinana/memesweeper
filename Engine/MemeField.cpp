#include "MemeField.h"
#include <assert.h>
#include <random>
#include "SpriteCodex.h"
#include <algorithm> 

void MemeField::Tile::Draw(const Vei2 screenPos, GameState gameState, Graphics& gfx) const
{
    if (gameState != MemeField::GameState::Fucked)
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
            if (HasMeme())
            {
                SpriteCodex::DrawTileMeme(screenPos, gfx);
            }
            else
            {
                SpriteCodex::DrawTileNumber(screenPos, nNeighborMemes, gfx);
            }
            break;
        }
    }
    else
    {
        switch (state)
        {
        case StateTypes::Hidden:
            if (HasMeme())
            {
                SpriteCodex::DrawTileMeme(screenPos, gfx);
            }
            else
            {
                SpriteCodex::DrawTileButton(screenPos, gfx);
            }
            break;
        case StateTypes::Flagged:
            //SpriteCodex::DrawTileButton(screenPos, gfx);
            if (HasMeme())
            {
                SpriteCodex::DrawTileButton(screenPos, gfx);
				SpriteCodex::DrawTileFlag(screenPos, gfx);
            }
            else
            {
                SpriteCodex::DrawTileMeme(screenPos, gfx);
                SpriteCodex::DrawTileCross(screenPos, gfx);
            }
            break;
        case StateTypes::Revealed:
            if (HasMeme())
            {
                SpriteCodex::DrawTileMemeRed(screenPos, gfx);
            }
            else
            {
                SpriteCodex::DrawTileNumber(screenPos, nNeighborMemes, gfx);
            }
            break;
        }
    }

}

void MemeField::Tile::Reveal()
{
    assert(state == StateTypes::Hidden);

    state = StateTypes::Revealed;
}

bool MemeField::Tile::IsRevealed() const
{
    return state == StateTypes::Revealed;
}

void MemeField::Tile::ToggleFlag()
{
    assert(state != StateTypes::Revealed);

    if (state != StateTypes::Flagged)
    {
        state = StateTypes::Flagged;
    }
    else
    {
        state = StateTypes::Hidden;
    }
}

bool MemeField::Tile::IsFlagged() const
{
    return state == StateTypes::Flagged;
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

void MemeField::Tile::SetNeighborMemeCount(int nMemes)
{
    assert(nNeighborMemes == -1);

    nNeighborMemes = nMemes;
}

int MemeField::Tile::GetNeighborMemeCount() const
{
    return nNeighborMemes;
}

RectI MemeField::GetRect() const
{
    return RectI(offset.x, (width * SpriteCodex::tileSize) + offset.x, offset.y, (height * SpriteCodex::tileSize) + offset.y);
}

MemeField::Tile& MemeField::TileAt(const Vei2 gridPos)
{
    return fields[gridPos.y * width + gridPos.x];
}

const MemeField::Tile& MemeField::TileAt(const Vei2 gridPos) const
{
    return fields[gridPos.y * width + gridPos.x];
}

MemeField::MemeField( int nMemes, Vei2 screenSize )
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

    CreateNeighborCount();

    offset = screenSize / 3;
}

void MemeField::Draw(Graphics& gfx) const
{
    RectI border = GetRect();
    border.left -= 20;
    border.right += 20;
    border.top -= 20;
    border.bottom += 20;
    gfx.DrawRect(border, Colors::Blue);
    gfx.DrawRect(GetRect(), SpriteCodex::baseColor);
    
    for (Vei2 gridPos = { 0,0 }; gridPos.y < height; ++gridPos.y)
    {
        for (gridPos.x = 0; gridPos.x < width; ++gridPos.x)
        {
            TileAt(gridPos).Draw((gridPos * SpriteCodex::tileSize) + offset, gameState, gfx);
		}
    }



    if (gameState == GameState::iWon)
    {
        SpriteCodex::DrawWin(offset, gfx);
    }
}

void MemeField::OnRevealClick(const Vei2 screenPos)
{
    if (gameState == GameState::SweepingMemes)
    {
        Vei2 gridPos = ScreenToGrid(screenPos - offset);
        RectI rect = GetRect();
        // if NOT REVEALED >:(
        assert(gridPos.x >= 0 && gridPos.x < width && gridPos.y >= 0 && gridPos.y < height);
        Tile& tile = TileAt(gridPos);
        if (!tile.IsRevealed() && !tile.IsFlagged())
        {
            if (tile.HasMeme())
            {
                gameState = GameState::Fucked;
            }
            tile.Reveal();
        }
    }

    bool likeReally = true;
    bool allSweeped = true;
    for (Tile& t : fields)
    {
        if (!t.HasMeme())
        {
            allSweeped = t.IsRevealed() && likeReally;
            likeReally = allSweeped;
            //likeReally = t.IsRevealed();
        }
    }

    if (allSweeped && likeReally)
    {
        gameState = GameState::iWon;
    }
}

void MemeField::OnFlagClick(const Vei2 screenPos)
{
    if (gameState == GameState::SweepingMemes)
    {
        Vei2 gridPos = ScreenToGrid(screenPos - offset);
        // if NOT REVEALED >:(
        assert(gridPos.x >= 0 && gridPos.x < width && gridPos.y >= 0 && gridPos.y < height);
        Tile& tile = TileAt(gridPos);
        if (!tile.IsRevealed())
        {
            tile.ToggleFlag();
        }
    }
}

Vei2 MemeField::ScreenToGrid(const Vei2 screenPos) const
{
    return screenPos / SpriteCodex::tileSize;
}

void MemeField::CreateNeighborCount()
{
    for (Vei2 checkPos = { 0, 0 }; checkPos.y < height; ++checkPos.y)
    {
        for (checkPos.x = 0; checkPos.x < width; ++checkPos.x)
        {
            TileAt(checkPos).SetNeighborMemeCount(GetTileNeighborCount(checkPos));
        }
    }
}

int MemeField::GetTileNeighborCount( const Vei2& gridPos )
{
    const int xStart = std::max(0, gridPos.x - 1);
    const int yStart = std::max(0, gridPos.y - 1);
    const int xEnd = std::min(width - 1, gridPos.x + 1);
    const int yEnd = std::min(height - 1, gridPos.y + 1);

    int count = 0;
    for ( Vei2 checkPos = {xStart, yStart}; checkPos.y <= yEnd; ++checkPos.y)
    {
        for (checkPos.x = xStart; checkPos.x <= xEnd; ++checkPos.x)
        {
            if (TileAt(checkPos).HasMeme())
            {
                ++count;
            }
        }
    }

    return count;
}
