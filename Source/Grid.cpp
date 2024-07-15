//==============================================================================
/*!
\file		Grid.cpp
\project		CS380/CS580 Group Project
\Team		wo AI ni
\summary		Definition of the Grid Class

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior
written consent of DigiPen Institute of Technology is prohibited.
*/
//==============================================================================

#include "Grid.h"
#include "Utility.h"
#include "Loader.h"
#include "Camera.h"
#include "Factory.h"
#include <algorithm>
#include <random>
#include <stack>

extern Factory factory;
extern sf::Font font;
extern Loader loader;
extern Camera camera;
//extern bool isDrawMode;
extern DrawMode mode;
extern float dt;

std::random_device rd;
std::mt19937 g(rd());

MapConfig config;

void drawArrow(sf::RenderWindow& window, Vec2 const& start, Vec2 const& direction, float length = 20.f, float headLength = 10.f)
{
    // Normalize the direction vector
    Vec2 normalizedDirection = direction.Normalize();

    // Calculate the end point of the arrow
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



Grid::Grid(int _height, int _width, float _cellSize)
	: height{ _height }, width{ _width }, cellSize { _cellSize }, cells{ static_cast<size_t>(height), std::vector<Cell>{ static_cast<size_t>(width) } }
	, flowField{ static_cast<size_t>(height), std::vector<flowFieldCell>{ static_cast<size_t>(width) } }
	, potentialField{ static_cast<size_t>(height), std::vector<potentialFieldCell>{ static_cast<size_t>(width)} }
{

	for (int row{}; row < height; ++row)
	{
		for (int col{}; col < width; ++col)
		{
			cells[row][col].rect.setOrigin(cellSize / 2.f, cellSize / 2.f);
			cells[row][col].rect.setSize(sf::Vector2f(cellSize, cellSize));
			cells[row][col].rect.setPosition(col * cellSize, row * cellSize); // was row *, col *
			cells[row][col].rect.setFillColor(colors.at("Floor").first);
			cells[row][col].rect.setOutlineColor(colors.at("Floor").second);
			cells[row][col].rect.setOutlineThickness(4.f);
			cells[row][col].pos = { row, col };

			// flow field
			flowField[row][col].position = { row, col };
			
			// potential field
			potentialField[row][col].direction = { 0, 0 };
			potentialField[row][col].potential = 1000;
			potentialField[row][col].position = { row, col };
		}
	}

	generateRandomGoal();
	//updatePotentialField();
}

// ======
// UPDATE
// ======
void Grid::render(sf::RenderWindow& window)
{
	//sf::Vector2f offset = camera.getOffset();

	for (Cell *cell : waypoints)
	{
		cell->intensity = 0.5f;
	}

	for (int row{}; row < height; ++row)
	{
		for (int col{}; col < width; ++col)
		{
			Cell& currCell = cells[row][col];

			// if cell is a wall but it has been explored before
			if (isWall(row, col))
			{
				if (currCell.visibility != UNEXPLORED || mode == DrawMode::WALL || mode == DrawMode::ENTITY)
				{
					//std::cout << "RENDERING EXPLORED WALLS\n";
					currCell.rect.setFillColor(colors.at("Wall").first);
					currCell.rect.setOutlineColor(colors.at("Wall").second);
				}

				else // Unexplored
				{
					//std::cout << "RENDERING UNEXPLORED WALLS\n";
					currCell.rect.setFillColor(colors.at("Unexplored").first);
					currCell.rect.setOutlineColor(colors.at("Unexplored").second);
				}

				if (currCell.isHighlighted)
				{
					currCell.rect.setOutlineColor(colors.at("Highlight").second);
					currCell.isHighlighted = false;
				}

				if (currCell.intensity > 0.f)
				{
					sf::Color color = currCell.rect.getFillColor();
					sf::Color hlColor = colors.at("Highlight").first;
					currCell.rect.setFillColor({ (sf::Uint8)((float)color.r + (float)hlColor.r *
						currCell.intensity), (sf::Uint8)((float)color.g + (float)hlColor.g * currCell.intensity),
						(sf::Uint8)((float)color.b + (float)hlColor.b * currCell.intensity) });
					currCell.intensity -= 0.5f * dt;
				}

				camera.addCell(currCell.rect);

				continue;
			}


			switch (cells[row][col].visibility)
			{
			case UNEXPLORED:
				// Black colour as unexplored colour
				currCell.rect.setFillColor(colors.at("Unexplored").first);
				currCell.rect.setOutlineColor(colors.at("Unexplored").second);
				break;

			case FOG:
				// Grey colour as fog colour
				currCell.rect.setFillColor(colors.at("Fog").first);
				currCell.rect.setOutlineColor(colors.at("Fog").second);
				break;

			case VISIBLE:
				// white colour as visible
				currCell.rect.setFillColor(colors.at("Visible").first);
				currCell.rect.setOutlineColor(colors.at("Visible").second);
				break;
			}

			if (currCell.isHighlighted)
			{
				currCell.rect.setOutlineColor(colors.at("Highlight").second);
				currCell.isHighlighted = false;
			}

			if (currCell.intensity > 0.f)
			{
				sf::Color color = currCell.rect.getFillColor();
				sf::Color hlColor = colors.at("Highlight").first;
				currCell.rect.setFillColor({ (sf::Uint8)((float)color.r + (float)hlColor.r *
					currCell.intensity), (sf::Uint8)((float)color.g + (float)hlColor.g * currCell.intensity),
					(sf::Uint8)((float)color.b + (float)hlColor.b * currCell.intensity) });
				currCell.intensity -= 0.5f * dt;
			}

			camera.addCell(currCell.rect);

			if (showHeatMap)
			{
				float value = flowField[row][col].distance;

				float normalizedDistance = std::min(1.f, value / 200.f); // Assuming max distance of 300 for normalization

				if (utl::isEqual(normalizedDistance, 1.f))
					continue;


				sf::Uint8 alpha = static_cast<sf::Uint8>((1.f - normalizedDistance) * 255);

				sf::Color color = sf::Color(255, 0, 0, alpha); // Red color with varying alpha
				currCell.rect.setFillColor(color);

				window.draw(currCell.rect);
		}
			

			// Checking if the cell is not a wall and has a valid direction
			if (flowFieldArrow)
			{
				if (!(utl::isEqual(flowField[row][col].distance, 0.f)))
				{
					Vec2 cellCenter = getWorldPos(row, col);
					Vec2 direction = flowField[row][col].direction;
					drawArrow(window, cellCenter, direction);
				}
			}

			if (showPotentialField)
			{
				float value = potentialField[row][col].potential;

				float normalizedDistance = value / 50.f; // Assuming max distance of 300 for normalization

				//if (utl::isEqual(normalizedDistance, 1.f))
				//	continue;


				sf::Uint8 alpha = static_cast<sf::Uint8>((normalizedDistance) * 255);

				sf::Color color = sf::Color(0, 0, 255, alpha); // Red color with varying alpha
				cells[row][col].rect.setFillColor(color);

				window.draw(cells[row][col].rect);
			}

#if 0 // TO DISPLAY THE NUMERICAL DISTANCE
			// Format the distance text to 2 decimal places
			sf::Text text;
			text.setFont(font);
			text.setCharacterSize(16);
			text.setFillColor(sf::Color::White);

			std::ostringstream ss;

			//ss << std::fixed << (int)flowField[row][col].direction.x << " " << (int)flowField[row][col].direction.y;
			ss << std::fixed << std::setprecision(2) << flowField[row][col].distance;

			text.setString(ss.str());

			

			// Center the text in the cell
			sf::FloatRect textRect = text.getLocalBounds();
			text.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
			text.setPosition(cells[row][col].rect.getPosition());

			// Draw the text
			window.draw(text);
#endif
		}
	}

	cells[exitCell->position.row][exitCell->position.col].rect.setFillColor(sf::Color(0, 255, 0, 255));
	window.draw(cells[exitCell->position.row][exitCell->position.col].rect);

	// only draw one debug circle for one entity
	if (debugDrawRadius)
		for (auto const& rad : debugRadius)
			window.draw(*rad);

	debugRadius.clear();

}

void Grid::updateVisibility(std::vector<std::pair<Vec2, Vec2>> const& entities, float fovRadius, float fovAngleDegrees, float visionCircleRadius)
{
	// Convert angle from degrees to radians
	float fovAngle = fovAngleDegrees * (PI / 180.0f);

	// Set all cells that are explored to FOG
	for (int row = 0; row < height; ++row)
	{
		for (int col = 0; col < width; ++col)
		{
			// Skip wall cells
			if (isWall(row, col))
				continue;

			if (cells[row][col].visibility != UNEXPLORED)
				cells[row][col].visibility = FOG;
		}
	}

	// For all entities in the vector
	for (const auto& entity : entities)
	{
		Vec2 p = entity.first;
		Vec2 direction = entity.second;

		GridPos gridpos = getGridPos(p);

		int startRow = std::max(0, gridpos.row - static_cast<int>(fovRadius / cellSize));
		int endRow = std::min(height - 1, gridpos.row + static_cast<int>(fovRadius / cellSize));

		int startCol = std::max(0, gridpos.col - static_cast<int>(fovRadius / cellSize));
		int endCol = std::min(width - 1, gridpos.col + static_cast<int>(fovRadius / cellSize));

		// Calculate the angle of the direction vector
		float directionAngle = atan2(direction.y, direction.x);
		float halfAngle = fovAngle / 2.0f;

		// Update visibility based on FOV
		for (int r = startRow; r <= endRow; ++r)
		{
			for (int c = startCol; c <= endCol; ++c)
			{
				Vec2 cellCenter = getWorldPos(r, c);
				Vec2 directionToCell = cellCenter - p;
				float distance = directionToCell.Distance(Vec2(0, 0));

				if (distance <= fovRadius)
				{
					// Calculate the angle between the direction vector and the vector to the cell
					float cellAngle = atan2(directionToCell.y, directionToCell.x);
					float relativeAngle = cellAngle - directionAngle;

					// Normalize the angle to the range [-PI, PI]
					if (relativeAngle > PI) relativeAngle -= 2 * PI;
					if (relativeAngle < -PI) relativeAngle += 2 * PI;

					// Check if the cell is within the vision cone
					if (fabs(relativeAngle) <= halfAngle)
					{
						if (!isClearPath(gridpos, GridPos{ r, c }))
							continue;

						if (cells[r][c].isExit)
						{
							exitFound = true;
						}

						cells[r][c].visibility = VISIBLE;

						if (!isWall(r, c))
						{
							cells[r][c].rect.setFillColor(colors.at("Visible").first);
							cells[r][c].rect.setOutlineColor(colors.at("Visible").second);
						}
					}
				}
			}
		}

		// Update visibility based on vision circle
		int visionStartRow = std::max(0, gridpos.row - static_cast<int>(visionCircleRadius / cellSize));
		int visionEndRow = std::min(height - 1, gridpos.row + static_cast<int>(visionCircleRadius / cellSize));

		int visionStartCol = std::max(0, gridpos.col - static_cast<int>(visionCircleRadius / cellSize));
		int visionEndCol = std::min(width - 1, gridpos.col + static_cast<int>(visionCircleRadius / cellSize));

		for (int r = visionStartRow; r <= visionEndRow; ++r)
		{
			for (int c = visionStartCol; c <= visionEndCol; ++c)
			{
				Vec2 cellCenter = getWorldPos(r, c);
				Vec2 directionToCell = cellCenter - p;
				float distance = directionToCell.Distance(Vec2(0, 0));

				if (distance <= visionCircleRadius)
				{
					if (!isClearPath(gridpos, GridPos{ r, c }))
						continue;

					if (cells[r][c].isExit)
					{
						exitFound = true;
					}

					cells[r][c].visibility = VISIBLE;

					if (!isWall(r, c))
					{
						cells[r][c].rect.setFillColor(colors.at("Visible").first);
						cells[r][c].rect.setOutlineColor(colors.at("Visible").second);
					}
				}
			}
		}

		// Debug draw if enabled
		if (debugDrawRadius)
		{
			if (utl::isEqual(fovAngleDegrees, 360.0f))
			{
				// Draw a circle if the angle is 360 degrees
				auto debugCircle = std::make_unique<sf::CircleShape>();
				debugCircle->setFillColor(colors.at("Debug_Radius").first);
				debugCircle->setOutlineColor(colors.at("Debug_Radius").second);
				debugCircle->setOutlineThickness(5.f);
				debugCircle->setRadius(fovRadius);
				debugCircle->setOrigin(fovRadius, fovRadius);
				debugCircle->setPosition(sf::Vector2f(p.x, p.y));
				debugRadius.push_back(std::move(debugCircle));
			}
			else
			{
				// Draw a cone if the angle is less than 360 degrees
				auto visionCone = std::make_unique<sf::ConvexShape>();
				visionCone->setPointCount(30);  // Increased to make it look like a cone
				visionCone->setPoint(0, sf::Vector2f(p.x, p.y));

				for (int i = 0; i <= 28; ++i)
				{
					float currentAngle = directionAngle - halfAngle + i * (fovAngle / 28);
					Vec2 point = Vec2(cos(currentAngle), sin(currentAngle)) * fovRadius;
					visionCone->setPoint(i + 1, sf::Vector2f(p.x + point.x, p.y + point.y));
				}

				visionCone->setFillColor(colors.at("Debug_Radius").first);
				visionCone->setOutlineColor(colors.at("Debug_Radius").second);
				visionCone->setOutlineThickness(5.f);

				debugRadius.push_back(std::move(visionCone));
			}

			// Draw the inner vision circle
			auto innerCircle = std::make_unique<sf::CircleShape>();
			innerCircle->setFillColor(sf::Color::Transparent);
			innerCircle->setOutlineColor(sf::Color::Red);
			innerCircle->setOutlineThickness(2.f);
			innerCircle->setRadius(visionCircleRadius);
			innerCircle->setOrigin(visionCircleRadius, visionCircleRadius);
			innerCircle->setPosition(sf::Vector2f(p.x, p.y));
			debugRadius.push_back(std::move(innerCircle));
		}
	}
}






void Grid::updateHeatMap(Vec2 target)
{
	// get target pos in gridPos
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
				if (isOutOfBound(neighbourPos) || isWall(neighbourPos) && cells[neighbourPos.row][neighbourPos.col].visibility != UNEXPLORED)
					continue;

				// skip diagonal neighbors if there's an adjacent wall
				if (i != 0 && j != 0)
				{
					GridPos adjacent1{ currCell.position.row + i, currCell.position.col };
					GridPos adjacent2{ currCell.position.row, currCell.position.col + j };
					if (isOutOfBound(adjacent1) || isOutOfBound(adjacent2) || isWall(adjacent1) && cells[adjacent1.row][adjacent1.col].visibility != UNEXPLORED || isWall(adjacent2) && cells[adjacent2.row][adjacent2.col].visibility)
						continue;
				}


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

void Grid::updateHeatMap()
{
	// get target pos in gridPos
	GridPos targetPos = exitCell->position;

	// skip out of bound target
	if (isOutOfBound(targetPos))
		return;

	// reset the map first
	resetHeatMap();

	// initialize target cell
	//flowField[targetPos.row][targetPos.col].distance = 0.f;
	//flowField[targetPos.row][targetPos.col].visited = true;

	// push unexplored node into openlist
	for (auto& row : flowField)
	{
		for (auto& flowFieldCell : row)
		{
			if (cells[flowFieldCell.position.row][flowFieldCell.position.col].visibility == UNEXPLORED)
			{
				flowFieldCell.distance = 0.f;
				flowFieldCell.visited = true;
				openList.push(&flowFieldCell);
			}
		}
	}

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
				if (isOutOfBound(neighbourPos) || isWall(neighbourPos) )
					continue;

				// skip diagonal neighbors if there's an adjacent wall
				if (i != 0 && j != 0)
				{
					GridPos adjacent1{ currCell.position.row + i, currCell.position.col };
					GridPos adjacent2{ currCell.position.row, currCell.position.col + j };
					if (isOutOfBound(adjacent1) || isOutOfBound(adjacent2) || isWall(adjacent1) && cells[adjacent1.row][adjacent1.col].visibility != UNEXPLORED || isWall(adjacent2) && cells[adjacent2.row][adjacent2.col].visibility)
						continue;
				}


				// get neighbour cells
				flowFieldCell& currNeighbour = flowField[neighbourPos.row][neighbourPos.col];


				// Calculate new distance
				float newDistance = currCell.distance + distOfTwoCells(currCell.position, neighbourPos);

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

void Grid::addRepulsion(GridPos gridPos, float radius, float strength)
{
	int startRow = std::max(0, gridPos.row - static_cast<int>(radius / cellSize));
	int endRow = std::min(height - 1, gridPos.row + static_cast<int>(radius / cellSize));

	int startCol = std::max(0, gridPos.col - static_cast<int>(radius / cellSize));
	int endCol = std::min(width - 1, gridPos.col + static_cast<int>(radius / cellSize));


	for (int r = startRow; r <= endRow; ++r)
	{
		for (int c = startCol; c <= endCol; ++c)
		{
			//if (isWall(r, c))
			//	continue;

			// get distance of pos to cell (startRow, startCol)

			Vec2 rowCol = getWorldPos(r, c);


			float distance = getWorldPos(gridPos).Distance(rowCol);

			if (distance <= radius)
			{
				if (!isClearPath(gridPos, GridPos{ r, c }))
					continue;

				flowField[r][c].distance += strength;
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
			if (isWall(row, col) && cells[row][col].visibility != UNEXPLORED)
				continue;

			
			// if goal node is found, we double break
			bool goalBreak{ false };
			
			// ===============================================
			// To check if there is wall amongst its neighbour
			// ===============================================
			bool minimumMode{ false }; // this mode is enabled if there is a wall among its neighbour
			float minDist{ std::numeric_limits<float>::max() };	// to store the min distancce for the case of minimumMode
			GridPos minDir{};

			// check its neighbour to generate direction vector
			for (int i{ -1 }; i <= 1; ++i)
			{
				for (int j{ -1 }; j <= 1; ++j)
				{
					int neighbourRow = row + i;
					int neighbourCol = col + j;

					// skip self cell
					if ( i == 0 && j == 0)
						continue;

					if (isOutOfBound(neighbourRow, neighbourCol))
					{
						minimumMode = true;
						continue;
					}

					// get current neighbour
					flowFieldCell& neighbourCell = flowField[neighbourRow][neighbourCol];


					// skip diagonal neighbors if there's an adjacent wall
					if (i != 0 && j != 0)
					{
						if (isOutOfBound(row + i, col) || isOutOfBound(row, col + j) || isWall(row + i, col) && cells[row + i][col].visibility != UNEXPLORED || isWall(row, col + j) && cells[row][col + j].visibility != UNEXPLORED)
						{
							minimumMode = true;
							continue;
						}
					}

					// if there is walls amongst neighbour
					if (isWall(neighbourRow, neighbourCol) && cells[neighbourRow][neighbourCol].visibility != UNEXPLORED || isOutOfBound(neighbourRow, neighbourCol)) // if there is a wall amongst its neighbour
					{
						minimumMode = true;
						continue;
					}
					
					// if pointing directly to goal node
					if (utl::isEqual(neighbourCell.distance, 0.f))
					{
						currCell.direction = Vec2((float)j, (float)i);
						goalBreak = true;
						minimumMode = false;
						break;
					}
						
					// for MINNIMUM MODE
					if (neighbourCell.distance < minDist)
					{
						minDist = neighbourCell.distance;
						minDir = { i, j };

					}

					// GRADIENT MODE
					// get final directional vector
					currCell.direction += (1.f / neighbourCell.distance) * Vec2((float)j, (float)i);
				}

				if (goalBreak)
					break;
			}

			if (minimumMode)
				currCell.direction = Vec2((float)minDir.col, (float)minDir.row);
			
		}
	}
}

void Grid::changeMap(const std::string& mapName)
{
	const std::vector<std::vector<bool>>& indexCells = loader.getMap(mapName);
	size_t newWidth = indexCells.size() ? indexCells[0].size() : 0;
	for (const std::vector<bool>& row : indexCells)
		crashIf(newWidth != row.size(), "Map " + utl::quote(mapName) + " has rows of different sizes");

	height = (int)indexCells.size();
	width = (int)newWidth;
	cells = std::vector<std::vector<Cell>>();
	flowField = std::vector<std::vector<flowFieldCell>>();

	for (unsigned i = 0; i < (unsigned)height; ++i)
	{
		cells.push_back(std::vector<Cell>());
		flowField.push_back(std::vector<flowFieldCell>());
		
		for (unsigned j = 0; j < (unsigned)width; ++j)
		{
			flowField.back().emplace_back();
			flowField.back().back().position = { (int)i, (int)j };
			cells.back().emplace_back(Vec2{ cellSize, cellSize });
			cells.back().back().isWall = indexCells[i][j];
			cells.back().back().pos = { (int)i, (int)j };

			sf::RectangleShape &currCell = cells.back().back().rect;
			currCell.setOrigin(cellSize / 2.f, cellSize / 2.f);
			currCell.setPosition(j * cellSize, i * cellSize);
			currCell.setFillColor(indexCells[i][j] ? colors.at("Wall").first : colors.at("Floor").first); 
			currCell.setOutlineColor(indexCells[i][j] ? colors.at("Wall").second : colors.at("Floor").second);
			currCell.setOutlineThickness(1.f);
		}
	}
}

void Grid::clearMap()
{
	for (std::vector<Cell> &row : cells)
		for (Cell &cell : row)
		{
			cell.isWall = false;
			cell.rect.setFillColor(colors.at("Floor").first);
			cell.rect.setFillColor(colors.at("Floor").second);
		}
}

void Grid::resetMap()
{
	for (std::vector<Cell> &row : cells)
		for (Cell &cell : row)
			cell.visibility = UNEXPLORED;

	for (Enemy *enemy : factory.getEntities<Enemy>())
		factory.destroyEntity<Enemy>(enemy);

	exitFound = false;
	resetHeatMap();
}

// potential field
void Grid::generateRandomGoal()
{
	srand(time(0));
	int exitX = rand() % width;
	int exitY = rand() % height;
	cells[exitY][exitX].isExit = true;

	exitCell = &potentialField[exitY][exitX];
	exitCell->potential = 0; // Exit has the lowest potential
}

void Grid::updatePotentialField()
{
	std::queue<potentialFieldCell> toProcess;
	toProcess.push(*exitCell);

	std::vector<std::pair<int, int>> directions = { {0, 1}, {1, 0}, {0, -1}, {-1, 0}, {1, 1}, {-1, 1}, {1, -1}, {-1, -1} };
	while (!toProcess.empty())
	{
		potentialFieldCell current = toProcess.front();
		toProcess.pop();

		for (auto &direction : directions) {
			int newX = current.position.col + direction.first;
			int newY = current.position.row + direction.second;

			if (!isOutOfBound(newY, newX))
			{
				potentialFieldCell &neighbor = potentialField[newY][newX];
				int newPotential = current.potential + 1;
				if (newPotential < neighbor.potential) {
					neighbor.potential = newPotential;
					toProcess.push(neighbor);
				}
			}
		}
	}
}
	// Add attractive forces towards unexplored areas
	//for (auto& row : potentialField) {
	//	for (auto& cell : row) {
	//		if (cells[cell.position.row][cell.position.col].visibility == Visibility::UNEXPLORED) {
	//			cell.potential = 100.f;
	//		}

	//		// Add repulsive forces from obstacles
	//		if (isWall(cell.position.row, cell.position.col))
	//		{
	//			cell.potential = 50.f;
	//		}
	//		else if (cells[cell.position.row][cell.position.col].visibility != Visibility::UNEXPLORED)
	//		{
	//			cell.potential = 20.f;
	//		}
	//	}
	//}

void Grid::generateMap()
{
	// so it doesn't crash on empty map
	if (cells.empty() || cells[0].empty())
		return;

	// initialise map
	for (std::vector<Cell> &row : cells)
		for (Cell &cell : row)
		{
			cell.isVisited = false;
			cell.connections = 0;
			cell.parents.clear();
			cell.parents.push_back(nullptr);
			cell.isWall = true;

			//if (config.deviation)
			//{
			//	float fx = utl::randInt(config.minConnections - 1, config.maxConnections - 1);
			//	float ex = getEx(cell.pos);

			//	cell.connections = utl::clamp(ex + (fx - ex) * (config.deviation / 10.f),
			//		config.minConnections - 1.f, config.maxConnections - 1.f);
			//	cell.visitsLeft = utl::round(cell.connections);
			//	//PRINT(cell.connections, cell.visitsLeft);
			//}
			//else
			cell.visitsLeft = utl::randInt(config.minConnections - 1, config.maxConnections - 1);
		}

	std::stack<Cell *> openList; // bfs
	openList.push(&cells[0][0]);

	while (openList.size())
	{
		Cell &currCell = *openList.top();
		openList.pop();

		currCell.isVisited = true;
		std::vector<Cell *> neighbors = getOrthNeighbors(currCell.pos, config.tunnelSize + 1);
		std::shuffle(neighbors.begin(), neighbors.end(), g);

		for (Cell *neighbor : neighbors)
			if (!neighbor->isVisited)
			{
				openList.push(neighbor);
				neighbor->parents[0] = &currCell;
			}
			else if (neighbor->visitsLeft-- > 0)
				neighbor->parents.push_back(&currCell);
	}

	// remove walls
#if 1
	for (std::vector<Cell> &row : cells)
		for (Cell &cell : row)
		{
			for (Cell *parent : cell.parents)
			{
				if (!parent)
					continue;
				//waypoints.push_back(&cell);
#if 1			
				if (parent->pos.row == cell.pos.row) // same row
				{
					int maxCol = std::max(parent->pos.col, cell.pos.col);
					int minCol = std::min(parent->pos.col, cell.pos.col);
					
					for (int j = 1; j < config.wallSize + 1; j = std::abs(j) + 1)
					{
						j = j % 2 ? -j : j;

						for (int i = minCol - config.wallSize / 2; i < minCol; ++i)
							if (!isOutOfBound(cell.pos.row + j / 2, i) && shouldEraseWall({ cell.pos.row + j / 2, i }, { cell.pos.row + j / 2, i - 1 }, i == minCol - config.wallSize / 2))
								cells[cell.pos.row + j / 2][i].isWall = false;

						if (!isOutOfBound(cell.pos.row + j / 2, maxCol))
							for (int i = minCol; i <= maxCol; ++i)
								//if (!(j / 2) || (utl::randInt(0, 10) - (i == minCol ? 0 : !cells[cell.pos.row + j / 2][i - 1].isWall) * 10) < 1)
								if (!(j / 2) || shouldEraseWall({ cell.pos.row + j / 2, i }, { cell.pos.row + j / 2, i - 1 }, false))
									cells[cell.pos.row + j / 2][i].isWall = false;
						/*if (!isOutOfBound(cell.pos.row + j / 2, maxCol + 1))
							cells[cell.pos.row + j / 2][maxCol + 1].isWall = false;*/

							for (int i = maxCol + config.wallSize / 2; i > maxCol; --i)
								if (!isOutOfBound(cell.pos.row + j / 2, i) && shouldEraseWall({ cell.pos.row + j / 2, i }, { cell.pos.row + j / 2, i - 1 }, i == minCol - config.wallSize / 2))
									cells[cell.pos.row + j / 2][i].isWall = false;
					}
				}
				else // same col
				{
					int maxRow = std::max(parent->pos.row, cell.pos.row);
					int minRow = std::min(parent->pos.row, cell.pos.row);

					for (int j = 1; j < config.wallSize + 1; j = std::abs(j) + 1)
					{
						j = j % 2 ? -j : j;

						for (int i = minRow - config.wallSize / 2; i < minRow; ++i)
							if (!isOutOfBound(i, cell.pos.col + j / 2) && shouldEraseWall({ i, cell.pos.col + j / 2 }, { i - 1, cell.pos.col + j / 2 }, i == minRow - config.wallSize / 2))
								cells[i][cell.pos.col + j / 2].isWall = false;

						if (!isOutOfBound(maxRow, cell.pos.col + j / 2))
							for (int i = minRow; i <= maxRow; ++i)
								//if (!(j / 2) || (utl::randInt(0, 10) - (i == minRow ? 0 : !cells[i - 1][cell.pos.col + j / 2].isWall) * 10) < 1)
								if (!(j / 2) || shouldEraseWall({ i, cell.pos.col + j / 2 }, { i - 1, cell.pos.col + j / 2 }, false))
									cells[i][cell.pos.col + j / 2].isWall = false;
						//if (!isOutOfBound(maxRow + 1, cell.pos.col + j / 2))
						//	cells[maxRow + 1][cell.pos.col + j / 2].isWall = false;

							for (int i = maxRow + config.wallSize / 2; i > maxRow; --i)
								if (!isOutOfBound(i, cell.pos.col + j / 2) && shouldEraseWall({ i, cell.pos.col + j / 2 }, { i - 1, cell.pos.col + j / 2 }, i == maxRow + config.wallSize / 2))
									cells[i][cell.pos.col + j / 2].isWall = false;
					}
				}
#endif
#if 0			
				if (cell.parent->pos.row == cell.pos.row) // same row
				{
					int maxCol = std::max(cell.parent->pos.col, cell.pos.col);

					for (int j = 1; j < 5; j = std::abs(j) + 1)
					{
						j = j % 2 ? -j : j;
						for (int i = std::min(cell.parent->pos.col, cell.pos.col) - (5 - 1) / 2; 
							i <= maxCol + (5 - 1) / 2; ++i)
							if (!isOutOfBound(cell.pos.row + j / 2, i))
								cells[cell.pos.row + j / 2][i].isWall = false;
						//if (!isOutOfBound(cell.pos.row + j / 2, maxCol + 1))
						//	cells[cell.pos.row + j / 2][maxCol + 1].isWall = false;
					}
				}
				else // same col
				{
					int maxRow = std::max(cell.parent->pos.row, cell.pos.row);

					for (int j = 1; j < 5; j = std::abs(j) + 1)
					{
						j = j % 2 ? -j : j;
						for (int i = std::min(cell.parent->pos.row, cell.pos.row) - (5 - 1) / 2;
							i <= maxRow + (5 - 1) / 2; ++i)
							if (!isOutOfBound(i, cell.pos.col + j / 2))
								cells[i][cell.pos.col + j / 2].isWall = false;
						//if (!isOutOfBound(maxRow + 1, cell.pos.col + j / 2))
						//	cells[maxRow + 1][cell.pos.col + j / 2].isWall = false;
					}
				}
#endif
#if 0
				if (cell.parent->pos.row == cell.pos.row) // same row
				{
					int maxCol = std::max(cell.parent->pos.col, cell.pos.col);
							for (int i = std::min(cell.parent->pos.col, cell.pos.col); i <= maxCol; ++i)
								cells[cell.pos.row][i].isWall = false;
				}
				else // same col
				{
					int maxRow = std::max(cell.parent->pos.row, cell.pos.row);
							for (int i = std::min(cell.parent->pos.row, cell.pos.row); i <= maxRow; ++i)
								cells[i][cell.pos.col].isWall = false;
				}
#endif
			}
		}
#endif
#if 1
	// reinitialise map
	for (std::vector<Cell> &row : cells)
		for (Cell &cell : row)
			cell.isVisited = false;

	while (openList.size())
		openList.pop();

	for (std::vector<Cell> &row : cells)
		for (Cell &cell : row)
			if (cell.isWall && !cell.isVisited/* && utl::randInt(0, 99) >= config.openness*/) 
			{
				openList.push(&cell);
				std::vector<Cell *> island;

				while (openList.size())
				{
					Cell &currCell = *openList.top();
					openList.pop();
					if (currCell.isVisited)
						continue;

					island.push_back(&currCell);
					currCell.isVisited = true;

					for (Cell *neighbor : getOrthNeighbors(currCell.pos, 1))
						if (neighbor->isWall)
							openList.push(neighbor);
				}

				if (island.size() < config.minIslandSize)
					for (Cell *wall : island)
						wall->isWall = false;
			}

#if 0
	// north border
	for (int i = 0; i < width; ++i)
		if (cells[0][i].isWall)
		{
			cells[0][i].isVisited = true;
			openList.push(&cells[0][i]);
		}

	// south border
	for (int i = 0; i < width; ++i)
		if (cells[height - 1][i].isWall)
		{
			cells[height - 1][i].isVisited = true;
			openList.push(&cells[height - 1][i]);
		}

	// east border
	for (int i = 0; i < height; ++i)
		if (cells[i][width - 1].isWall)
		{
			cells[i][width - 1].isVisited = true;
			openList.push(&cells[i][width - 1]);
		}

	// west border
	for (int i = 0; i < height; ++i)
		if (cells[i][0].isWall)
		{
			cells[i][0].isVisited = true;
			openList.push(&cells[i][0]);
		}
#endif

	//while (openList.size())
	//{
	//	Cell &currCell = *openList.top();
	//	openList.pop();
	//	//if (currCell.isVisited)
	//	//	continue;

	//	currCell.isVisited = true;
	//	for (Cell *neighbor : getOrthNeighbors(currCell.pos, 1))
	//		if (neighbor->isWall && !neighbor->isVisited)
	//			openList.push(neighbor);
	//}
		
	for (std::vector<Cell> &row : cells)
		for (Cell &cell : row)
			if (!cell.isVisited)
				cell.isWall = false;
#endif
}

bool Grid::shouldEraseWall(GridPos currCell, GridPos prevCell, bool isFirst)
{
	return utl::randInt(1, 20) -
		(config.isEqualWidth ? (isFirst ? 1 : 
			(isOutOfBound(prevCell) ? 1 : !cells[prevCell.row][prevCell.col].isWall)) :
			(isOutOfBound(prevCell) ? 1 : !cells[prevCell.row][prevCell.col].isWall)) *
		(20 - config.noise * 2) <= config.noise;
}

typename Grid::potentialFieldCell Grid::getNextMove(Vec2 pos)
{
	std::vector<std::pair<int, int>> directions = { {0, 1}, {1, 0}, {0, -1}, {-1, 0}, {1, 1}, {-1, 1}, {1, -1}, {-1, -1} };
	potentialFieldCell nextMove = potentialField[pos.y][pos.x];

	for (auto& direction : directions) {
		int newX = pos.x + direction.first;
		int newY = pos.y + direction.second;

		if (!isOutOfBound(newY, newX)) {
			potentialFieldCell& neighbor = potentialField[newY][newX];
			if (!isWall(neighbor.position) && cells[newY][newX].visibility == Visibility::VISIBLE && neighbor.potential < nextMove.potential) {
				nextMove = neighbor;
			}
		}
	}

	return nextMove;
}


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

GridPos Grid::getGridPos(float x, float y) const
{
	x += 0.5f * cellSize;
	y += 0.5f * cellSize;

	int row = static_cast<int>(y / cellSize);
	int col = static_cast<int>(x / cellSize);

	return { row, col };
}

GridPos Grid::getGridPos(Vec2 const& pos) const 
{ 
	return getGridPos(pos.x, pos.y); 
}


Vec2 Grid::getWorldPos(int row, int col) const
{
	return Vec2{ col * cellSize, row * cellSize };
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

std::vector<Cell *> Grid::getOrthNeighbors(GridPos pos, int steps)
{
	std::vector<Cell *> ret;

	// north
	if (!isOutOfBound(pos.row - steps, pos.col))
		ret.push_back(&cells[pos.row - steps][pos.col]);
	
	// south
	if (!isOutOfBound(pos.row + steps, pos.col))
		ret.push_back(&cells[pos.row + steps][pos.col]);

	// east
	if (!isOutOfBound(pos.row, pos.col + steps))
		ret.push_back(&cells[pos.row][pos.col + steps]);

	// west
	if (!isOutOfBound(pos.row, pos.col - steps))
		ret.push_back(&cells[pos.row][pos.col - steps]);

	return ret;
}

std::vector<Cell *> Grid::getNeighborWalls(GridPos pos)
{
	std::vector<Cell *> ret;

	for (int i = pos.row - 1; i < pos.row + 2; ++i)
		for (int j = pos.col - 1; j < pos.col + 2; ++j)
			if (!isOutOfBound(i, j) && cells[i][j].isWall)
				ret.push_back(&cells[i][j]);

	return ret;
}

float Grid::getEx(GridPos pos)
{
	float ex = 0.f;

	// north west
	ex += isOutOfBound(pos.row - 1, pos.col - 1) ? static_cast<float>(utl::randInt(
		config.minConnections - 1, config.maxConnections - 1)) : cells[pos.row - 1][pos.col - 1].connections;

	// north
	ex += isOutOfBound(pos.row - 1, pos.col) ? static_cast<float>(utl::randInt(
		config.minConnections - 1, config.maxConnections - 1)) : cells[pos.row - 1][pos.col].connections;

	// west
	ex += isOutOfBound(pos.row, pos.col - 1) ? static_cast<float>(utl::randInt(
		config.minConnections - 1, config.maxConnections - 1)) : cells[pos.row][pos.col - 1].connections;

	return ex / 3.f;
}

// =======
// SETTERS
// =======

void Grid::setVisibility(int row, int col, Visibility visibility)
{
	cells[row][col].visibility = visibility;
}

void Grid::setVisibility(GridPos pos, Visibility visibility) { return setVisibility(pos.row, pos.col, visibility); }

void Grid::SetOutlineColour(int row, int col, sf::Color colour)
{
	if (isOutOfBound(row, col))
		return;
	cells[row][col].rect.setOutlineColor(colour);
}

void Grid::SetOutlineColour(GridPos pos, sf::Color colour) { SetOutlineColour(pos.row, pos.col, colour); }

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
	SetColour(row, col, colors.at(penColour).first);
	SetOutlineColour(row, col, colors.at(penColour).second);
}

void Grid::SetColour(GridPos pos) 
{  
	if (!colors.count(penColour))
		return;
	SetColour(pos, colors.at(penColour).first);
	SetOutlineColour(pos, colors.at(penColour).second);
}

void Grid::setPenColour(const std::string &colourName) { penColour = colourName; }

void Grid::setHighlight(GridPos pos)
{
	if (isOutOfBound(pos))
		return;
	cells[pos.row][pos.col].isHighlighted = true;
}

void Grid::setIntensity(GridPos pos, float _intensity)
{
	if (isOutOfBound(pos))
		return;
	cells[pos.row][pos.col].intensity = _intensity;
}

void Grid::setWidth(int newWidth)
{
	if (width == newWidth)
		return;
	bool isSmaller = newWidth < width;

	for (int i = 0; i < height; ++i)
		if (isSmaller)
		{
			cells[i].erase(cells[i].begin() + newWidth, cells[i].end());
			flowField[i].erase(flowField[i].begin() + newWidth, flowField[i].end());
		}
		else
			for (int j = width; j < newWidth; ++j)
			{
				Cell cell;
				cell.rect.setOrigin(cellSize / 2.f, cellSize / 2.f);
				cell.rect.setSize(sf::Vector2f(cellSize, cellSize));
				cell.rect.setPosition(j * cellSize, i * cellSize);
				cell.rect.setFillColor(colors.at("Floor").first);
				cell.rect.setOutlineColor(colors.at("Floor").second);
				cell.rect.setOutlineThickness(4.f);
				cell.pos = { i, j };
				cells[i].push_back(cell);

				flowField[i].emplace_back();
				flowField[i].back().position = { i, j };
			}

	width = newWidth;
}

void Grid::setHeight(int newHeight)
{
	if (height == newHeight)
		return;
	bool isSmaller = newHeight < height;

	if (isSmaller)
	{
		cells.erase(cells.begin() + newHeight, cells.end());
		flowField.erase(flowField.begin() + newHeight, flowField.end());
	}
	else
		for (int i = height; i < newHeight; ++i)
		{
			cells.push_back(std::vector<Cell>());
			flowField.push_back(std::vector<flowFieldCell>());

			for (int j = 0; j < width; ++j)
			{
				Cell cell;
				cell.rect.setOrigin(cellSize / 2.f, cellSize / 2.f);
				cell.rect.setSize(sf::Vector2f(cellSize, cellSize));
				cell.rect.setPosition(j * cellSize, i * cellSize);
				cell.rect.setFillColor(colors.at("Floor").first);
				cell.rect.setOutlineColor(colors.at("Floor").second);
				cell.rect.setOutlineThickness(4.f);
				cell.pos = { i, j };
				cells.back().push_back(cell);

				flowField.back().emplace_back();
				flowField.back().back().position = { i, j };
			}
		}

	height = newHeight;
}

void Grid::setWall(GridPos pos, bool _isWall)
{
	setWall(pos.row, pos.col, _isWall);
}

void Grid::setWall(int row, int col, bool _isWall)
{
	if (isOutOfBound(row, col))
		return;

	cells[row][col].isWall = _isWall;
	SetColour(row, col, _isWall ? colors.at("Wall").first : colors.at("Floor").first);
}


// ========
// CHECKERS
// ========

const std::vector<std::vector<Cell>> &Grid::getCells() const
{
	return cells;
}

bool Grid::isWall(unsigned int row, unsigned int col) const
{

	if (isOutOfBound(row, col))
	{
		std::cout << "Out Of Bound!!!\n"; 
		return true;
	}
	//crashIf(isOutOfBound(row, col), "Row: " + utl::quote(std::to_string(row)) + " Col: " + utl::quote(std::to_string(col)) + " is out of bound because Height: " + utl::quote(std::to_string(height)) + " Width: " + utl::quote(std::to_string(width)));

	// assuming wall colour is black
	//return cells[row][col].rect.getFillColor() == colors.at("Wall").first;

	return cells[row][col].isWall;
}

bool Grid::isWall(GridPos pos) const { return isWall(pos.row, pos.col); }


bool Grid::isOutOfBound(int row, int col) const
{
	return row >= height || col >= width || row < 0 || col < 0;
}

bool Grid::isOutOfBound(GridPos pos) const { return isOutOfBound(pos.row, pos.col); }


bool Grid::isClearPath(int row0, int col0, int row1, int col1) const
{
	//if (isWall(row0, col0) || isWall(row1, col1))
	//	return false;

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
			if (!isWall(row, col))
				continue;

			if (row == row0 && col == col0 || row == row1 && col == col1)
				continue;
			
			

				

			if ((row == row1 && col == col1) || (row == row0 && col == col0))
				continue;

			// convert minRow and minCol to vector 3D
			Vec2 wallCenter = getWorldPos(row, col);

			// get points of diagonal
			Vec2 l1p0 = { wallCenter.x - halfGridWidth, wallCenter.y + halfGridWidth };
			Vec2 l1p1 = { wallCenter.x + halfGridWidth, wallCenter.y - halfGridWidth };
			Vec2 l2p0 = { wallCenter.x + halfGridWidth, wallCenter.y + halfGridWidth };
			Vec2 l2p1 = { wallCenter.x - halfGridWidth, wallCenter.y - halfGridWidth };


			if (lineIntersect(p0, p1, l1p0, l1p1) || lineIntersect(p0, p1, l2p0, l2p1))
				return false;
		}
	}
	return true;

}

bool Grid::isClearPath(GridPos lhs, GridPos rhs) const
{
	return isClearPath(lhs.row, lhs.col, rhs.row, rhs.col);
}


bool Grid::lineIntersect(const Vec2& line0P0, const Vec2& line0P1, const Vec2& line1P0, const Vec2& line1P1) const
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

Cell::Cell(Vec2 pos) : rect(pos) { }

std::ostream &operator<<(std::ostream &os, GridPos const &rhs)
{
	os << "{" << rhs.row << ", " << rhs.col << "}";
	return os;
}

bool operator==(GridPos lhs, GridPos rhs)
{
	return lhs.row == rhs.row && lhs.col == rhs.col;
}