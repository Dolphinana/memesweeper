#pragma once
#include "Vei2.h"
#include "Graphics.h"

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
		void Draw( const Vei2 screenPos, Graphics& gfx) const;
		void Reveal();
		bool Revealed() const;
		void SpawnMeme();
		bool HasMeme() const;
	private:
		StateTypes state = StateTypes::Hidden;
		bool hasMeme = false;
	};
private:
	RectI GetRect() const;
	Tile& TileAt( const Vei2 gridPos );
	const Tile& TileAt(const Vei2 gridPos) const;
public:
	MemeField( int nMemes );
	void Draw( Graphics& gfx ) const;
	void OnRevealClick(const Vei2 screenPos);

	Vei2 ScreenToGrid(const Vei2 screenPos) const;
private:
	static constexpr int width = 20;
	static constexpr int height = 16;
	Tile fields[width * height];

};