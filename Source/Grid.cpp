//! FILE HEADER

#include "Grid.h"
#include "Utility.h"
#include "Loader.h"
#include "Camera.h"

extern sf::Font font;
extern Loader loader;
extern Camera camera;

bool Grid::hasLineOfSight(Vec2 const& start, Vec2 const& end) const {
	int x0 = static_cast<int>(start.x / cellSize);
	int y0 = static_cast<int>(start.y / cellSize);
	int x1 = static_cast<int>(end.x / cellSize);
	int y1 = static_cast<int>(end.y / cellSize);

	int dx = abs(x1 - x0);
	int dy = abs(y1 - y0);
	int sx = x0 < x1 ? 1 : -1;
	int sy = y0 < y1 ? 1 : -1;
	int err = dx - dy;

	while (true) {
		if (isWall(y0, x0)) return false; // If there's a wall, return false

		if (x0 == x1 && y0 == y1) return true; // If we've reached the end, return true

		int e2 = 2 * err;
		if (e2 > -dy) {
			err -= dy;
			x0 += sx;
		}
		if (e2 < dx) {
			err += dx;
			y0 += sy;
		}
	}
}

bool line_intersect(const Vec2& line0P0, const Vec2& line0P1, const Vec2& line1P0, const Vec2& line1P1)
{
	const float y4y3 = line1P1.y - line1P0.y;
	const float y1y3 = line0P0.y - line1P0.y;
	const float y2y1 = line0P1.y - line0P0.y;
	const float x4x3 = line1P1.x - line1P0.x;
	const float x2x1 = line0P1.x - line0P0.x;
	const float x1x3 = line0P0.x - line1P0.x;

	const float divisor = y4y3 * x2x1 - x4x3 * y2y1;
	const float dividend0 = x4x3 * y1y3 - y4y3 * x1x3;
	const float dividend1 = x2x1 * y1y3 - y2y1 * x1x3;

	const float eps = 0.0001f;
	if (std::abs(dividend0) < eps && std::abs(dividend1) < eps && std::abs(divisor) < eps)
	{	// Lines coincident (on top of each other)
		return true;
	}

	if (std::abs(divisor) < eps)
	{	// Lines parallel
		return false;
	}

	const float quotient0 = dividend0 / divisor;
	const float quotient1 = dividend1 / divisor;

	if (quotient0 < 0.0f || quotient0 > 1.0f || quotient1 < 0.0f || quotient1 > 1.0f)
	{	// No intersection
		return false;
	}
	else
	{	// Intersection
		return true;
	}
}

void drawArrow(sf::RenderWindow& window, Vec2 const& start, Vec2 const& direction, float length = 20.f, float headLength = 10.f)
{
	// Normalize the direction vector
	Vec2 normalizedDirection = direction.Normalize();

	// Calculate the end point of the arrow
	//Vec2 start = start + camera.getOffset();
	Vec2 end = start + normalizedDirection * length;

	// Create the main line of the arrow
	sf::VertexArray arrow(sf::LinesStrip, 3);
	arrow[0].position = sf::Vector2f(start.x, start.y);
	arrow[1].position = sf::Vector2f(end.x, end.y);

	// Calculate the head of the arrow
	Vec2 leftHead = end - normalizedDirection * headLength + Vec2(-normalizedDirection.y, normalizedDirection.x) * headLength / 2;
	Vec2 rightHead = end - normalizedDirection * headLength + Vec2(normalizedDirection.y, -normalizedDirection.x) * headLength / 2;

	// Add the head lines
	arrow[2].position = sf::Vector2f(leftHead.x, leftHead.y);
	arrow.append(sf::Vertex(sf::Vector2f(end.x, end.y)));
	arrow.append(sf::Vertex(sf::Vector2f(rightHead.x, rightHead.y)));

	window.draw(arrow);
}

//Grid::Grid(int _width, int _height, float _cellSize)
//	: width(_width), height(_height), cellSize(_cellSize), cells(height, std::vector<Cell>(width)),
//	flowField(height, std::vector<flowFieldCell>(width))

Grid::Grid(int _width, int _height, float _cellSize)
	: width{ _width }, height{ _height }, cellSize{ _cellSize }, cells{ static_cast<size_t>(height), std::vector<Cell>{ static_cast<size_t>(width) } }, flowField{ static_cast<size_t>(height), std::vector<flowFieldCell>{ static_cast<size_t>(width) } }
{
	for (int row{}; row < height; ++row)
	{
		for (int col{}; col < width; ++col)
		{
			cells[row][col].rect.setSize(sf::Vector2f(cellSize, cellSize));
			cells[row][col].rect.setPosition(row * cellSize, col * cellSize);
			cells[row][col].rect.setFillColor(colors.at("Floor_Fill"));
			cells[row][col].rect.setOutlineColor(colors.at("Floor_Outline"));
			cells[row][col].rect.setOutlineThickness(4.f);

			flowField[row][col].position = { row, col };
		}
	}

	// initialize debug pov
	debugRadius.setFillColor(colors.at("Debug_Radius_Fill"));
	debugRadius.setOutlineColor(colors.at("Debug_Radius_Outline"));
	debugRadius.setOutlineThickness(5.f);


	//TEMP GRID MAKING
   // Left vertical part of U
	for (int row = 10; row < 10 + 10; ++row)
		cells[row][10].rect.setFillColor(colors.at("Wall_Fill"));
	

	// Bottom horizontal part of U
	for (int col = 10; col < 10 + 10; ++col) 
		cells[19][col].rect.setFillColor(colors.at("Wall_Fill"));
	

	// Right vertical part of U
	for (int row = 10; row < 10 + 10; ++row) 
		cells[row][19].rect.setFillColor(colors.at("Wall_Fill"));
	

}

// ======
// UPDATE
// ======
void Grid::render(sf::RenderWindow& window)
{
	//sf::Vector2f offset = camera.getOffset();

	for (int row{}; row < height; ++row)
	{
		for (int col{}; col < width; ++col)
		{
			// draw and skip walls
			if (isWall(row, col))
			{
				//sf::Vector2f oldPos = cells[row][col].rect.getPosition();
				//cells[row][col].rect.setPosition(oldPos + offset);
				//window.draw(cells[row][col].rect);
				//cells[row][col].rect.setPosition(oldPos);
				camera.addCell(cells[row][col].rect);
				continue;
			}

			switch (cells[row][col].visibility)
			{
			case UNEXPLORED:
				// Black colour as unexplored colour
				cells[row][col].rect.setFillColor(colors.at("Unexplored_Fill"));
				cells[row][col].rect.setOutlineColor(colors.at("Unexplored_Outline"));
				break;

			case FOG:
				// Grey colour as fog colour
				cells[row][col].rect.setFillColor(colors.at("Fog_Fill"));
				cells[row][col].rect.setOutlineColor(colors.at("Fog_Outline"));
				break;

			case VISIBLE:
				// white colour as visible
				cells[row][col].rect.setFillColor(colors.at("Visible_Fill"));
				cells[row][col].rect.setOutlineColor(colors.at("Visible_Outline"));
				break;
			}

			//sf::Vector2f oldPos = cells[row][col].rect.getPosition();
			//cells[row][col].rect.setPosition(oldPos + offset);
			//window.draw(cells[row][col].rect);
			//cells[row][col].rect.setPosition(oldPos);
			//camera.addRectangle(cells[row][col].rect);
			camera.addCell(cells[row][col].rect);


			if (debugDrawRadius)
			{
				//sf::Vector2f oldPos = debugRadius.getPosition();
				//debugRadius.setPosition(oldPos + offset);
				window.draw(debugRadius);
				//debugRadius.setPosition(oldPos);
			}


#if 0
			//!!! TEMP
			float value = flowField[row][col].distance;

			float normalizedDistance = std::min(1.f, value / 200.f); // Assuming max distance of 300 for normalization

			if (utl::isEqual(normalizedDistance, 1.f))
				continue;


			sf::Uint8 alpha = static_cast<sf::Uint8>((1.f - normalizedDistance) * 255);

			sf::Color color = sf::Color(255, 0, 0, alpha); // Red color with varying alpha
			cells[row][col].rect.setFillColor(color);

			window.draw(cells[row][col].rect);

#endif

			if (!isWall(row, col) && !(!flowField[row][col].direction.x && !flowField[row][col].direction.y))
			{
				Vec2 cellCenter = getWorldPos(row, col) + Vec2(cellSize / 2.0f, cellSize / 2.0f);
				drawArrow(window, cellCenter, flowField[row][col].direction * (cellSize / 2.0f));
			}

			//// Format the distance text to 2 decimal places
			//sf::Text text;
			//text.setFont(font);
			//text.setCharacterSize(16);
			//text.setFillColor(colors.at("Floor_Fill"));

			//std::ostringstream ss;

			//ss << std::fixed << (int)flowField[row][col].direction.x << " " << (int)flowField[row][col].direction.y;
			////ss << std::fixed << std::setprecision(2) << flowField[row][col].distance;

			//text.setString(ss.str());

			//

			//// Center the text in the cell
			//sf::FloatRect textRect = text.getLocalBounds();
			//text.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
			//text.setPosition(cells[row][col].rect.getPosition() + sf::Vector2f(cellSize / 2.0f, cellSize / 2.0f));

			//// Draw the text
			//window.draw(text);



			if (debugDrawRadius)
				window.draw(debugRadius);
		}
	}
}

void Grid::updateVisibility(std::vector<Vec2> const& pos, float radius)
{
	// set all cells that are explored to FOG
	for (int row{}; row < height; ++row)
		for (int col{}; col < width; ++col)
		{
			// skip wall cells
			if (isWall(row, col))
				continue;

			if (cells[row][col].visibility != UNEXPLORED)
				cells[row][col].visibility = FOG;
		}

	// for all positions in the position vector
	for (Vec2 const& p : pos)
	{
		GridPos gridpos = getGridPos(p);

		int startRow = std::max(0, gridpos.row - static_cast<int>(radius / cellSize));
		int endRow = std::min(height - 1, gridpos.row + static_cast<int>(radius / cellSize));

		int startCol = std::max(0, gridpos.col - static_cast<int>(radius / cellSize));
		int endCol = std::min(width - 1, gridpos.col + static_cast<int>(radius / cellSize));


		for (int r = startRow; r <= endRow; ++r)
		{
			for (int c = startCol; c <= endCol; ++c)
			{
				if (isWall(r, c))
					break;

				// get distance of pos to cell (startRow, startCol)
				float distance = p.Distance(getWorldPos(r, c));

				if (distance <= radius && hasLineOfSight(p, getWorldPos(r, c)))
				{
					cells[r][c].visibility = VISIBLE;
					cells[r][c].rect.setFillColor(colors.at("Visible_Fill"));
					cells[r][c].rect.setOutlineColor(colors.at("Visible_Outline"));
				}
			}
		}


		debugRadius.setRadius(radius);
		debugRadius.setOrigin(radius, radius);
		debugRadius.setPosition(sf::Vector2(p.x, p.y));
	}



}



void Grid::updateHeatMap(Vec2 target)
{
	// get target pos in gridPos
	//GridPos targetPos = getGridPos(target - camera.getOffset() * canUseCameraOffset);
	GridPos targetPos = getGridPos(target);

	// skip out of bound target
	if (isOutOfBound(targetPos))
		return;

	// reset the map first
	resetHeatMap();


	// initialize target cell
	flowField[targetPos.row][targetPos.col].distance = 0.f;
	flowField[targetPos.row][targetPos.col].visited = true;

	// push target node into openlist
	openList.push(&flowField[targetPos.row][targetPos.col]);

	while (!openList.empty())
	{
		flowFieldCell& currCell = *openList.front();

		openList.pop();

		// check its 8 neighour
		for (int i{ -1 }; i <= 1; ++i)
		{
			for (int j{ -1 }; j <= 1; ++j)
			{
				// skip self cell
				if (i == 0 && j == 0)
					continue;

				// create neighour position
				GridPos neighbourPos{ currCell.position.row + i, currCell.position.col + j };

				// skip out of bound and wall cells
				if (isOutOfBound(neighbourPos) || isWall(neighbourPos))
					continue;

				// get neighbour cells
				flowFieldCell& currNeighbour = flowField[neighbourPos.row][neighbourPos.col];


				// Calculate new distance
				float newDistance = currCell.distance + distOfTwoCells(targetPos, neighbourPos);

				// If neighbor is visited and the new distance is shorter, update it
				if (currNeighbour.visited)
				{
					if (newDistance < currNeighbour.distance)
						currNeighbour.distance = newDistance;
				}
				else
				{
					// If neighbor is not visited, set the distance and mark as visited
					currNeighbour.distance = newDistance;
					currNeighbour.visited = true;
					openList.push(&currNeighbour);
				}
			}
		}
	}
}


void Grid::resetHeatMap()
{
	for (int row{}; row < height; ++row)
	{
		for (int col{}; col < width; ++col)
		{
			flowField[row][col].distance = std::numeric_limits<float>::max();
			flowField[row][col].visited = false;
			flowField[row][col].direction = { 0, 0 };
		}
	}
}


void Grid::generateFlowField()
{
	for (int row{}; row < height; ++row)
	{
		for (int col{}; col < width; ++col)
		{
			// get current cells
			flowFieldCell &currCell = flowField[row][col];

			if (utl::isEqual(currCell.distance, 0.f))
				continue;

			// Skip walls
			if (isWall(row, col))
			{
				//currCell.direction = Vec2(0, 0);
				continue;
			}

			// check its neighbour to generate direction vector
			for (int i{ -1 }; i <= 1; ++i)
			{
				for (int j{ -1 }; j <= 1; ++j)
				{
					int neighbourRow = row + i;
					int neighbourCol = col + j;

					// skip out of bound, walls and self cell
					if (isOutOfBound(neighbourRow, neighbourCol) || i == 0 && j == 0)
						continue;

					flowFieldCell &neighbourCell = flowField[neighbourRow][neighbourCol];

					if (utl::isEqual(neighbourCell.distance, 0.f))
					{
						currCell.direction += Vec2((float)i, (float)j);
						continue;
					}


					// get final directional vector
					currCell.direction += (1.f / neighbourCell.distance) * Vec2((float)i, (float)j);
				}
			}

			//! test to get closest int

			//currCell.direction = currCell.direction.Normalize();



			// Round the direction components to the nearest integer
			//currCell.direction.x = std::round(currCell.direction.x);
			//currCell.direction.y = std::round(currCell.direction.y);

			// Ensure the direction components are clamped to valid values (-1, 0, 1)
			//currCell.direction.x = std::max(-1.f, std::min(currCell.direction.x, 1.f));
			//currCell.direction.y = std::max(-1.f, std::min(currCell.direction.y, 1.f));
		}
	}
}

void Grid::setColor(unsigned int row, unsigned int col, const sf::Color& color)
{
	//crashIf(isOutOfBound(row, col), "Row: " + utl::quote(std::to_string(row)) + " Col: " + utl::quote(std::to_string(col)) + " is out of bound");
	if (isOutOfBound(row, col))
		return;

	cells[row][col].rect.setFillColor(color);
}

void Grid::setColor(Vec2 pos, const sf::Color& color)
{
	//pos -= camera.getOffset();
	int row = static_cast<int>(pos.x / cellSize);
	int col = static_cast<int>(pos.y / cellSize);
	if (isOutOfBound(row, col))
		return;
	//crashIf(isOutOfBound(row, col), "Row: " + utl::quote(std::to_string(row)) + " Col: " + utl::quote(std::to_string(col)) + " is out of bound");

	cells[row][col].rect.setFillColor(color);
}

void Grid::changeMap(const std::string& mapName)
{
	const std::vector<std::vector<std::string>>& indexCells = loader.getMap(mapName);
	size_t newHeight = indexCells.size() ? indexCells[0].size() : 0;
	for (const std::vector<std::string>& row : indexCells)
		crashIf(newHeight != row.size(), "Map " + utl::quote(mapName) + " has rows of different sizes");

	width = (int)indexCells.size();
	height = (int)newHeight;
	cells = std::vector<std::vector<Cell>>();

	for (unsigned i = 0; i < (unsigned)width; ++i)
	{
		cells.push_back(std::vector<Cell>());

		for (unsigned j = 0; j < (unsigned)height; ++j)
		{
			cells.back().emplace_back(Vec2{ cellSize, cellSize });
			sf::RectangleShape &currCell = cells.back().back().rect;

			currCell.setPosition(i * cellSize, j * cellSize);
			currCell.setFillColor(colors.at(indexCells[i][j])); // guaranteed to not crash
			currCell.setOutlineColor(colors.at("Floor_Outline"));
			currCell.setOutlineThickness(1.f);
		}
	}
}

void Grid::clearMap()
{
	for (std::vector<Cell> &row : cells)
		for (Cell &cell : row)
			cell.rect.setFillColor(colors.at("Floor_Fill"));
}

//void Grid::computePath(Entity& entity, vec2 target) const
//{
//	entity.setTargetPos(target);
//
//	while (entity.pos != target)
//	{
//		// get current cell based on where entity is on
//		auto[row, col] = getGridPos(entity.pos);
//
//		// set direction based on flow field 
//		entity.dir = flowField[row][col].direction;
//	}
//}


// =======
// GETTERS
// =======

float Grid::getCellSize() const
{
	return cellSize;
}

int Grid::getWidth() const
{
	return width;
}

int Grid::getHeight() const
{
	return height;
}

Grid::GridPos Grid::getGridPos(float x, float y) const
{
	//x -= camera.getOffset().x * canUseCameraOffset;
	//y -= camera.getOffset().y * canUseCameraOffset;
	int row = static_cast<int>(x / cellSize);
	int col = static_cast<int>(y / cellSize);

	return { row, col };
}

Grid::GridPos Grid::getGridPos(Vec2 const& pos) const 
{ 
	//Vec2 pos = pos - camera.getOffset() * canUseCameraOffset;
	return getGridPos(pos.x, pos.y); 
}


Vec2 Grid::getWorldPos(int row, int col) const
{
	return Vec2{ row * cellSize, col * cellSize };
}


Vec2 Grid::getWorldPos(GridPos pos) const { return getWorldPos(pos.row, pos.col); }



float Grid::distOfTwoCells(GridPos lhs, GridPos rhs) const
{
	// Calculate the Euclidean distance
	float dx = (float)lhs.row - rhs.row;
	float dy = (float)lhs.col - rhs.col;

	return std::sqrt(dx * dx + dy * dy);
}

Vec2 Grid::getFlowFieldDir(int row, int col) const
{
	return flowField[row][col].direction;
}

Vec2 Grid::getFlowFieldDir(GridPos pos) const { return getFlowFieldDir(pos.row, pos.col); }




// =======
// SETTERS
// =======

void Grid::setVisibility(int row, int col, Visibility visibility)
{
	cells[row][col].visibility = visibility;
}

void Grid::setVisibility(GridPos pos, Visibility visibility) { return setVisibility(pos.row, pos.col, visibility); }


void Grid::SetColour(int row, int col, sf::Color colour)
{
	if (isOutOfBound(row, col))
		return;
	//crashIf(isOutOfBound(row, col), "Row: " + utl::quote(std::to_string(row)) + " Col: " + utl::quote(std::to_string(col)) + " is out of bound");

	cells[row][col].rect.setFillColor(colour);
}

void Grid::SetColour(GridPos pos, sf::Color colour) { SetColour(pos.row, pos.col, colour); }

void Grid::SetColour(int row, int col)
{
	if (!colors.count(penColour))
		return;
	SetColour(row, col, colors.at(penColour));
}

void Grid::SetColour(GridPos pos) 
{  
	if (!colors.count(penColour))
		return;
	SetColour(pos, colors.at(penColour));
}

void Grid::setPenColour(const std::string &colourName) { penColour = colourName; }

// ========
// CHECKERS
// ========

const std::vector<std::vector<Cell>> &Grid::getCells() const
{
	return cells;
}

bool Grid::isWall(unsigned int row, unsigned int col) const
{
	crashIf(isOutOfBound(row, col), "Row: " + utl::quote(std::to_string(row)) + " Col: " + utl::quote(std::to_string(col)) + " is out of bound");

	// assuming wall colour is black
	return cells[row][col].rect.getFillColor() == colors.at("Wall_Fill");
}

bool Grid::isWall(GridPos pos) const { return isWall(pos.row, pos.col); }


bool Grid::isOutOfBound(int row, int col) const
{
	return row >= height || col >= width || row < 0 || col < 0;
}

bool Grid::isOutOfBound(GridPos pos) const { return isOutOfBound(pos.row, pos.col); }


bool Grid::isClearPath(int row0, int col0, int row1, int col1) const
{
	if (isWall(row0, col0) || isWall(row1, col1))
		return false;

	// convert row0 and col0 to vectors 2D
	Vec2 p0 = getWorldPos(row0, col0);

	// convert row1 and col1 to vectors 2D
	Vec2 p1 = getWorldPos(row1, col1);

	// just use width since map is a square
	float halfGridWidth = 0.5f * cellSize;

	// puff up the grid width by the epsilon scale of itself
	halfGridWidth += halfGridWidth * EPSILON;

	// find min and max
	int sx = row0 < row1 ? 1 : -1;
	int sy = col0 < col1 ? 1 : -1;

	for (int row = row0; (sx > 0) ? (row <= row1) : (row >= row1); row += sx)
	{
		for (int col = col0; (sy > 0) ? (col <= col1) : (col >= col1); col += sy)
		{
			// skip non-wall cells
			if (isWall(row, col))
				continue;

			// convert minRow and minCol to vector 3D
			Vec2 wallCenter = getWorldPos(row, col);

			// get points of diagonal
			Vec2 l1p0 = { wallCenter.x - halfGridWidth, wallCenter.y + halfGridWidth };
			Vec2 l1p1 = { wallCenter.x + halfGridWidth, wallCenter.y - halfGridWidth };
			Vec2 l2p0 = { wallCenter.x + halfGridWidth, wallCenter.y + halfGridWidth };
			Vec2 l2p1 = { wallCenter.x - halfGridWidth, wallCenter.y - halfGridWidth };


			if (line_intersect(p0, p1, l1p0, l1p1) || line_intersect(p0, p1, l2p0, l2p1))
				return false;
		}
	}
	return true;

}

bool Grid::isClearPath(GridPos lhs, GridPos rhs) const
{
	return isClearPath(lhs.row, lhs.col, rhs.row, rhs.col);
}

Cell::Cell(Vec2 pos) : rect(pos) { }