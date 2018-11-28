#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1PathFinding.h"

j1PathFinding::j1PathFinding() : j1Module(), map(NULL), last_path(DEFAULT_PATH_LENGTH),width(0), height(0)
{
	name.create("pathfinding");
}

// Destructor
j1PathFinding::~j1PathFinding()
{
	RELEASE_ARRAY(map);
}

// Called before quitting
bool j1PathFinding::CleanUp()
{
	LOG("Freeing pathfinding library");

	last_path.Clear();
	RELEASE_ARRAY(map);
	return true;
}

// Sets up the walkability map
void j1PathFinding::SetMap(uint width, uint height, uchar* data)
{
	this->width = width;
	this->height = height;

	RELEASE_ARRAY(map);
	map = new uchar[width*height];
	memcpy(map, data, width*height);
}

// Utility: return true if pos is inside the map boundaries
bool j1PathFinding::CheckBoundaries(const iPoint& pos) const
{
	return (pos.x >= 0 && pos.x <= (int)width &&
			pos.y >= 0 && pos.y <= (int)height);
}

// Utility: returns true is the tile is walkable
bool j1PathFinding::IsWalkable(const iPoint& pos) const
{
	uchar t = GetTileAt(pos);
	return t != INVALID_WALK_CODE && t > 0;
}

// Utility: return the walkability value of a tile
uchar j1PathFinding::GetTileAt(const iPoint& pos) const
{
	if(CheckBoundaries(pos))
		return map[(pos.y*width) + pos.x];

	return INVALID_WALK_CODE;
}

// To request all tiles involved in the last generated path
const p2DynArray<iPoint>* j1PathFinding::GetLastPath() const
{
	return &last_path;
}

// PathList ------------------------------------------------------------------------
// Looks for a node in this list and returns it's list node or NULL
// ---------------------------------------------------------------------------------
p2List_item<PathNode>* PathList::Find(const iPoint& point) const
{
	p2List_item<PathNode>* item = list.start;
	while(item)
	{
		if(item->data.pos == point)
			return item;
		item = item->next;
	}
	return NULL;
}

// PathList ------------------------------------------------------------------------
// Returns the Pathnode with lowest score in this list or NULL if empty
// ---------------------------------------------------------------------------------
p2List_item<PathNode>* PathList::GetNodeLowestScore() const
{
	p2List_item<PathNode>* ret = NULL;
	int min = 65535;

	p2List_item<PathNode>* node = list.end;
	while(node)
	{
		if(node->data.Score() < min)
		{
			min = node->data.Score();
			ret = node;
		}
		node = node->prev;
	}
	return ret;
}

// PathNode -------------------------------------------------------------------------
// Convenient constructors
// ----------------------------------------------------------------------------------
PathNode::PathNode() : g(-1), h(-1), pos(-1, -1), parent(NULL)
{}

PathNode::PathNode(int g, int h, const iPoint& pos, const PathNode* parent) : g(g), h(h), pos(pos), parent(parent)
{}

PathNode::PathNode(const PathNode& node) : g(node.g), h(node.h), pos(node.pos), parent(node.parent)
{}

// PathNode -------------------------------------------------------------------------
// Fills a list (PathList) of all valid adjacent pathnodes
// ----------------------------------------------------------------------------------
uint PathNode::FindWalkableAdjacents(PathList& list_to_fill) const
{
	iPoint cell;
	uint before = list_to_fill.list.count();

	// Up
	cell.create(pos.x, pos.y + 1);
	if (App->pathfinding->IsWalkable(cell))
		list_to_fill.list.add(PathNode(-1, -1, cell, this));

	// Right
	cell.create(pos.x + 1, pos.y);
	if (App->pathfinding->IsWalkable(cell))
		list_to_fill.list.add(PathNode(-1, -1, cell, this));

	// Down
	cell.create(pos.x, pos.y - 1);
	if (App->pathfinding->IsWalkable(cell))
		list_to_fill.list.add(PathNode(-1, -1, cell, this));

	// Left
	cell.create(pos.x - 1, pos.y);
	if (App->pathfinding->IsWalkable(cell))
		list_to_fill.list.add(PathNode(-1, -1, cell, this));

	//// Up Left
	//cell.create(pos.x + 1, pos.y + 1);
	//if (App->pathfinding->IsWalkable(cell))
	//	list_to_fill.list.add(PathNode(-1, -1, cell, this));

	//// Up Right
	//cell.create(pos.x - 1, pos.y + 1);
	//if (App->pathfinding->IsWalkable(cell))
	//	list_to_fill.list.add(PathNode(-1, -1, cell, this));

	//// Down Left
	//cell.create(pos.x - 1, pos.y - 1);
	//if (App->pathfinding->IsWalkable(cell))
	//	list_to_fill.list.add(PathNode(-1, -1, cell, this));

	//// Down Right
	//cell.create(pos.x + 1, pos.y - 1);
	//if (App->pathfinding->IsWalkable(cell))
	//	list_to_fill.list.add(PathNode(-1, -1, cell, this));

	return list_to_fill.list.count();
}

// PathNode -------------------------------------------------------------------------
// Calculates this tile score
// ----------------------------------------------------------------------------------
int PathNode::Score() const
{
	return g + h;
}

// PathNode -------------------------------------------------------------------------
// Calculate the F for a specific destination tile
// ----------------------------------------------------------------------------------
int PathNode::CalculateF(const iPoint& destination)
{
	uint movCost = 1;

	g = parent->g + movCost;
	h = pos.DistanceTo(destination);
	//h = pos.DistanceNoSqrt(destination);

	return g + h;
}

// ----------------------------------------------------------------------------------
// Actual A* algorithm: return number of steps in the creation of the path or -1 ----
// ----------------------------------------------------------------------------------
int j1PathFinding::CreatePath(const iPoint& origin, const iPoint& destination)
{
	// Base return = no path
	int ret = -1;

	// Origin and destination must be walkable and not the same tile
	if (!(!IsWalkable(origin) || !IsWalkable(destination) || origin == destination)) {

		// Declare the lists
		PathList open;		// To visit nodes
		PathList closed;	// Visited nodes
		PathList adjacent;	// Adjecent nodes to current

		// Add origin to open list
		open.list.add(PathNode(0, 0, origin, NULL));

		// Iterate until no more tiles are left to visit
		while (open.list.count() > 0) {
			p2List_item<PathNode>* lowest = open.GetNodeLowestScore();			// Get the lowest score node from open
			p2List_item<PathNode>* currNode = closed.list.add(lowest->data);	// Move it to closed
			open.list.del(lowest);												// Delete it from open

			// If current node == destination, draw new path
			if (currNode->data.pos == destination) {
				last_path.Clear();
				
				// Backtrack to fill the path list (destination -> origin)
				for (const PathNode* pathNode = &currNode->data; pathNode != nullptr; pathNode = pathNode->parent){
					last_path.PushBack(pathNode->pos);
				}

				last_path.Flip();	// Flip list so: origin -> destination
			}
			else {
				// Fill a list with all adjacent nodes to currNode
				adjacent.list.clear();
				currNode->data.FindWalkableAdjacents(adjacent);

				// Iterate for all adjecent tiles
				for (p2List_item<PathNode>* currAdjecent = adjacent.list.start; currAdjecent != nullptr; currAdjecent = currAdjecent->next) {

					// Proceed if adjecent tile is NOT in closed list
					if (closed.Find(currAdjecent->data.pos) == NULL) {
						p2List_item<PathNode>* openAdjecent = open.Find(currAdjecent->data.pos);

						// If adjecent is not in open, calculateF and add it to open
						if (openAdjecent == NULL) {
							currAdjecent->data.CalculateF(destination);
							open.list.add(currAdjecent->data);
						}

						// Else, compare its G with the current node's G, if it's lower
						else if (openAdjecent->data.g < currNode->data.g) {
							openAdjecent->data.parent = currNode->data.parent;
							openAdjecent->data.CalculateF(destination);
						}
					}
				}
			}
		}
	}

	return ret;

	//// TODO 1: if origin or destination are not walkable, return -1
	//if (IsWalkable(origin) == true && IsWalkable(destination) == true) {

	//	int stepCount = 0;

	//	// TODO 2: Create two lists: open, close
	//	PathList open;
	//	PathList close;

	//	iPoint startDistance = { destination.x - origin.x, destination.y - origin.y };

	//	if (startDistance.x < 0)
	//		startDistance.x *= -1;

	//	if (startDistance.y < 0)
	//		startDistance.x *= -1;

	//	PathNode originNode(0, startDistance.x + startDistance.y, origin, nullptr);

	//	// Add the origin tile to open
	//	open.list.add(originNode);

	//	// Iterate while we have tile in the open list
	//	while (open.list.count() > 0) {

	//		// TODO 3: Move the lowest score cell from open list to the closed list
	//		close.list.add(open.GetNodeLowestScore()->data);
	//		open.list.del(open.GetNodeLowestScore());

	//		// TODO 4: If we just added the destination, we are done!
	//		// Backtrack to create the final path
	//		// Use the Pathnode::parent and Flip() the path when you are finish
	//		// CHANGE/FIX

	//		// TODO 5: Fill a list of all adjancent nodes

	//		// TODO 6: Iterate adjancent nodes:
	//		// ignore nodes in the closed list
	//		// If it is NOT found, calculate its F and add it to the open list
	//		// If it is already in the open list, check if it is a better path (compare G)
	//		// If it is a better path, Update the parent
	//	}
	//}
		
	//return -1;
}

