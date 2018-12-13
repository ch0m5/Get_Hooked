#ifndef __j1USERINTERFACE_H__
#define __j1USERINTERFACE_H__

#include "j1Module.h"
#include "p2List.h"

#define CURSOR_WIDTH 2

enum class ui_type;
class UIElement;
class Image;
class Text;
class Window;
struct _TTF_Font;
struct SDL_Rect;
struct SDL_Texture;

class j1UserInterface : public j1Module
{
public:

	j1UserInterface();

	// Destructor
	virtual ~j1UserInterface();

	// Called when before render is available
	bool Awake(pugi::xml_node&);

	// Call before first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	// Called each frame (logic)
	bool UpdateTick(float dt);

	// Called each frame (graphic)
	bool Draw();

	// Called after all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();
	
public:
	SDL_Texture* GetAtlas() const;
	
	void AddElement(UIElement* element);
	void DestroyElement(p2List_item<UIElement*>* element);

	UIElement* CreateImage(fPoint center, SDL_Rect texRect = { 0, 0, 0, 0 }, SDL_Texture* tex = NULL, bool dynamic = false, UIElement* parent = NULL, p2List<UIElement*>* children = NULL);
	UIElement* CreateText(fPoint center, const char* content, SDL_Color color = { 255, 255, 255, 255 }, _TTF_Font* font = NULL, bool dynamic = false, UIElement* parent = NULL, p2List<UIElement*>* children = NULL);

	UIElement* CreateActionBox(void(*action)(void), fPoint center, SDL_Rect spriteList[4], SDL_Texture* tex, bool dynamic = false, UIElement* parent = NULL, p2List<UIElement*>* children = NULL);
	//UIElement* CreateCheckBox();
	//UIElement* CreateInputText();

	//Window* CreateWindowPanel(fPoint center, p2List<Image*> children, SDL_Rect* texRect = NULL, SDL_Texture* tex = NULL, Text* label = NULL, UIElement* parent = NULL);

	/*Image* CreateText(const char* content, fPoint position, SDL_Color color = { 255, 255, 255, 255 }, _TTF_Font* font = NULL, Image* parent = NULL);
	Image* CreateWindowBox(fPoint position, iPoint size, SDL_Rect texRect, p2List<Image*> elemList, SDL_Texture* tex = NULL, Image* parent = NULL);
	Image* CreateActionBox(Button<void>::buttonAction action, fPoint position, Text* label, SDL_Rect* spriteList[4], image_type type, SDL_Texture* tex = NULL, Image* parent = NULL);
	Image* CreateCheckBox();
	Image* CreateInputText();*/
	
public:
	bool mustDebugDraw = false;

private:
	pugi::xml_node configNode;
	p2List<UIElement*> screenElements;

	SDL_Texture* atlas;
	p2SString atlas_file_name;
};

#endif // __j1USERINTERFACE_H__