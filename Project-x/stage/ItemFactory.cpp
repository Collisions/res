//
//  ItemFactory.cpp
//  ProX
//
//  Created by  YueBinbin on 14-6-4.
//
//

#include "ItemFactory.h"
#include "pugixml.hpp"
#include "resource.h"
#include "Tools.h"

#define GET_COLOR(color) getColor(str2num(color))
#define SET_COLOR(node, color) node->setColor(getColor(str2num(color)))

static ItemFactory* itemFactory = nullptr;

ItemFactory* ItemFactory::getInstance()
{
    if (!itemFactory)
    {
        itemFactory = new ItemFactory();
        itemFactory->init();
    }
    
    return itemFactory;
}

void ItemFactory::init()
{
    
}

void ItemFactory::createStage(Node &parent, const std::string &filename)
{
    for (auto ritem: itemArr)
    {
        ritem->removeFromParent();
    }
    itemArr.clear();

    
    pugi::xml_document* doc;
    long size;
    std::string filePath = FileUtils::getInstance()->fullPathForFilename(filename);
    //log("=>%s", filePath);
    unsigned char* data = FileUtils::getInstance()->getFileData(filePath, "rb", &size);
    
    if(!data)
    {
        CCLOG("read file error!");
        createStage(parent, "stage_1.xml");
    }
    
    doc = new pugi::xml_document();
    //pugi::xml_parse_result result =
    doc->load_buffer(data, size);
    CC_SAFE_DELETE(data);
    
    pugi::xml_node root = doc->child("stage");
    
    auto bg = parent.getChildByTag(BG);
    auto ground = parent.getChildByTag(GROUND);
    auto player = parent.getChildByTag(PLAYER);
    SET_COLOR(bg, root.attribute("bgColor").as_string());
    SET_COLOR(ground, root.attribute("gColor").as_string());
    SET_COLOR(player, root.attribute("boxColor").as_string());
    
    pugi::xml_node items = root.child("items");
    
    for (pugi::xml_node item = items.child("item"); item;  item = item.next_sibling("item"))
    {
        
        auto ite = LayerColor::create(Color4B::RED, str2num(item.attribute("width").as_string()), str2num(item.attribute("height").as_string()));
        setPositionByStr(*ite, item.attribute("position").as_string());
        SET_COLOR(ite, item.attribute("color").as_string());
        ite->setTag(str2num(item.attribute("tag").as_string()));
        parent.addChild(ite);
        itemArr.pushBack(ite);
        
        pugi::xml_node action = item.child("action");
        if (action) {
            createAction(*ite, str2num(action.attribute("type").as_string()),str2num(action.attribute("dura").as_string()), action.attribute("para").as_string());
        }
    }
    
    ground->setZOrder(parent.getChildrenCount());
}

Color3B ItemFactory::getColor(int color)
{
    switch (color) {
        case 1:
            return Color3B::BLACK;
        case 2:
            return Color3B::RED;
        case 3:
            return Color3B::YELLOW;
        case 4:
            return Color3B::ORANGE;
        case 5:
            return Color3B::MAGENTA;
        case 6:
            return Color3B::GREEN;
        case 7:
            return Color3B::GRAY;
        case 8:
            return Color3B::BLUE;
        case 9:
            return Color3B::WHITE;
		case 10:
			return Color3B::RBLUE;
        default:
            return Color3B::WHITE;
    }
}

float ItemFactory::str2num(const std::string &str)
{
    float f;
    std::stringstream ff(str);
    ff>>f;
    return f;
}

void ItemFactory::setPositionByStr(Node &node, const std::string &pos)
{
    unsigned long index = pos.find_first_of(",");
    
    std::string www = pos.substr(0, index);
    std::string hhh = pos.substr(index + 1, pos.length());
    
    int width,height;
    std::stringstream ww(www);
    ww>>width;
    std::stringstream hh(hhh);
    hh>>height;
    
    auto size = Director::getInstance()->getVisibleSize();
    
    node.setPosition(Point(size.width * width/100, size.height * height/100 + 150));
    
}

void ItemFactory::createAction(Node &node, int type, float dura, const std::string &para)
{
    
    if(type == 1)
    {
        unsigned long index = para.find_first_of(",");
        
        std::string www = para.substr(0, index);
        std::string hhh = para.substr(index + 1, para.length());
        
        int width,height;
        std::stringstream ww(www);
        ww>>width;
        std::stringstream hh(hhh);
        hh>>height;

        auto size = Director::getInstance()->getVisibleSize();

        auto move = MoveBy::create(dura, Point(size.width * width/100, size.height * height/100));
        auto move_back = move->reverse();
        auto seq = Sequence::create(move, move_back, NULL);
        auto rep = RepeatForever::create(seq);
        node.runAction(rep);
    } else if (type == 2)
    {
        unsigned long index = para.find_first_of(",");
        
        std::string www = para.substr(0, index);
        std::string hhh = para.substr(index + 1, para.length());
        
        float width,height;
        std::stringstream ww(www);
        ww>>width;
        std::stringstream hh(hhh);
        hh>>height;

        auto actionBy = ScaleBy::create(dura, width, height);
        auto seq = Sequence::create(actionBy, actionBy->reverse(), NULL);
        auto rep = RepeatForever::create(seq);
        node.runAction(rep);
    } else if (type == 3)
    {
        auto action = FadeOut::create(dura);
        auto seq = Sequence::create(action, action->reverse(), NULL);
        auto rep = RepeatForever::create(seq);
        node.runAction(rep);
    } else if (type == 4)
    {
        unsigned long index = para.find_first_of(",");
		
        std::string www = para.substr(0, index);
        std::string hhh = para.substr(index + 1, para.length());
		
        float width, height;
        std::stringstream ww(www);
        ww >> width;
        std::stringstream hh(hhh);
        hh >> height;
		
        auto size = Director::getInstance()->getVisibleSize();
				
        node.runAction(Sequence::create(
            MoveBy::create(dura, Point(size.width * width / 100, size.height * height / 100)),
					CallFuncN::create(CC_CALLBACK_1(ItemFactory::scale, this)),NULL));
			
    } else if (type == 5)
    {
        std::vector<std::string> vec = Tools::getInstance()->splitStr(para, ":");
		
        auto array = PointArray::create(20);
        for (std::string sub : vec)
        {
            array->addControlPoint(getPoint(sub));
        }
		    
        auto action = CardinalSplineBy::create(dura, array, 0);
        auto reverse = action->reverse();
		    
        auto seq = Sequence::create(action, reverse, NULL);
        node.runAction(seq);
    }else if (type == 6)
    {
        std::vector<std::string> vec = Tools::getInstance()->splitStr(para, ":");
        std::string temp = vec[0].c_str();
        unsigned long index = temp.find_first_of(",");
				
        int w = str2num(temp.substr(0, index));
        int h = str2num(temp.substr(index + 1, temp.length()));

				auto action = JumpBy::create(dura, Point(w, h), str2num(vec[1].c_str()), str2num(vec[2].c_str()));
				node.runAction(action);
		}else if (type == 7)	//иак╦
		{
				auto action = Blink::create(dura, str2num(para));
				node.runAction(action);
    }else if (type == 8)
    {
        std::vector<std::string> vec = Tools::getInstance()->splitStr(para, ":");
        std::string temp = vec[1].c_str();
        
        unsigned long index = temp.find_first_of(",");
        
        int w = str2num(temp.substr(0, index));
        int h = str2num(temp.substr(index + 1, temp.length()));
        
        auto size = Director::getInstance()->getVisibleSize();
        
        auto stay = MoveBy::create(str2num(vec[0].c_str()), Point(0, 0));
        auto move = MoveBy::create(dura, Point(size.width * w/100, size.height * h/100));
       
        auto seq = Sequence::create(stay, move, NULL);
        node.runAction(seq);
    }

}
    
void ItemFactory::scale(Node* node)
{
	node->stopAllActions(); //After this stop next action not working, if remove this stop everything is working
	node->runAction(ScaleTo::create(50, 50));
}

Point ItemFactory::getPoint(const std::string &str)
{
    unsigned long index = str.find_first_of(",");
    
    std::string www = str.substr(0, index);
    std::string hhh = str.substr(index + 1, str.length());
    
    int width,height;
    std::stringstream ww(www);
    ww>>width;
    std::stringstream hh(hhh);
    hh>>height;
    
    auto size = Director::getInstance()->getVisibleSize();
    log("x====%f, y====%f", size.width * width/100, size.height * height/100 + 150);
    return Point(size.width * width/100, size.height * height/100);
}


