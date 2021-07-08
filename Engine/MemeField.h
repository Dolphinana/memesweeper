#pragma once
#include "Vei2.h"
#include "Graphics.h"
#include "RectI.h"

class MemeField
{
private:
	class Tile
	{
	public:
		enum class StateTypes
		{
			Hidden,
			Flagged,
			Revealed
		};
	public:
		void Draw( const Vei2 screenPos, bool isFucked, Graphics& gfx) const;
		void Reveal();
		bool IsRevealed() const;
		void ToggleFlag();
		bool IsFlagged() const;
		void SpawnMeme();
		bool HasMeme() const;
		void SetNeighborMemeCount(int nMemes);
		int GetNeighborMemeCount() const;
	private:
		int nNeighborMemes = -1;
		StateTypes state = StateTypes::Hidden;
		bool hasMeme = false;
	};
private:
	Tile& TileAt( const Vei2 gridPos );
	const Tile& TileAt(const Vei2 gridPos) const;
public:
	MemeField( int nMemes );
	void Draw( Graphics& gfx ) const;
	void OnRevealClick(const Vei2 screenPos);
	void OnFlagClick(const Vei2 screenPos);
	RectI GetRect() const;
	Vei2 ScreenToGrid(const Vei2 screenPos) const;

	void CreateNeighborCount();
	int GetTileNeighborCount( const Vei2& gridPos );
private:
	static constexpr int width = 20;
	static constexpr int height = 16;
	Tile fields[width * height];
	bool isFucked = false;
};