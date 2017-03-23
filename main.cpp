#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <map>
class Factory
{
    private:
    int soldierCount;
    int ownership;
    int production;
    
    public:
    Factory(int id);
    int ID;
    void setOwnerShip(int owner);
    bool isMine();
    bool isNeutral();
    bool isEnemy();
    void setSoldierCount(int soldiers);
    int getSoldierCount();
    void setProduction(int prod);
    int getProduction();
    void increaseProduction();
    std::map<int, int> links; //ID, Distance in turns
    void printLinks();
};

Factory::Factory(int id)
{
    this->ID = id;
    this->ownership = 0;
    this->production = 0;
}

void Factory::setOwnerShip(int owner)
{
    this->ownership = owner;
}

bool Factory::isMine()
{
    return this->ownership == 1;   
}

bool Factory::isNeutral()
{
    return this->ownership == 0;
}

bool Factory::isEnemy()
{
    return this->ownership == -1;
}

void Factory::setSoldierCount(int soldiers)
{
    this->soldierCount = soldiers;
}

int Factory::getSoldierCount()
{
    return this->soldierCount;
}

void Factory::setProduction(int prod)
{
    this->production = prod;
}

int Factory::getProduction()
{
    return this->production;
}

void Factory::increaseProduction()
{
    if(getProduction() == 3) return;
    
    setProduction(getProduction() + 1);
}

void Factory::printLinks()
{
    std::cerr << "For factory " << this->ID << std::endl;
    for(std::map<int, int>::iterator it = links.begin(); it != links.end(); it++)
    {
        std::cerr << it->first << " " << it->second << std::endl;
    }
    std::cout << std::endl;
}

class Troop
{
    public:
    int ownership;
    int sourceID;
    int targetID;
    int strength;
    int arrivalTime;
};

class TroopManager
{
    public:
    std::vector<Troop*> troops;
    int amountOfEnemiesComing(Factory* target);
};

class Graph
{
    public:
    std::vector<Factory*> factories;
    bool contains(int id);
    Factory* getFactory(int id);
    int weakestEnemyFactoryID();
    int closestAvailableFactory(int factoryID);
    int biggestEnemyFactory();
    int getBomberIdeal(int id);
    int weakestProductiveFactory();
    int closestProductiveFactory(int id);
    int closestFriendlyUnproductive(int id);
    int closestFriendInNeed(int id, TroopManager* tm);
    int soldiersToSend(int srcId, int targetId, TroopManager* t);
};

int Graph::soldiersToSend(int srcId, int targetId, TroopManager* t)
{
    if(srcId == -1 || targetId == -1) return 0;
    
    Factory* f = getFactory(srcId);
    Factory* e = getFactory(targetId);

	int d = f->links[e->ID];
    					
	int solds = -1;
	if(e->isEnemy())
		    solds = 1 + e->getSoldierCount() + e->getProduction() * (d + 1) + t->amountOfEnemiesComing(e) + t->amountOfEnemiesComing(f);
	else solds = 1 + e->getSoldierCount() + t->amountOfEnemiesComing(e) + t->amountOfEnemiesComing(f);	
	return solds;    	
}

bool Graph::contains(int id)
{
    return getFactory(id) != NULL;
}

Factory* Graph::getFactory(int id)
{
    std::vector<Factory*>::iterator it;
    for(it = factories.begin(); it < factories.end(); it++)
    {
        if(id == (*it)->ID)
            return (*it);
    }
    return NULL;
}

int Graph::weakestEnemyFactoryID()
{
    int weakestId = -1;
    int lessForces = 100000;
    for(std::vector<Factory*>::iterator it = factories.begin(); it < factories.end(); it++)
    {
        if((*it)->getSoldierCount() <= lessForces && !((*it)->isMine()))
        {
            weakestId = (*it)->ID;
            lessForces = (*it)->getSoldierCount();
        }
    }
    return weakestId;
}

int Graph::closestProductiveFactory(int id)
{
    Factory* f = getFactory(id);
    int closestID = -1;
    int closestDis = 100000;
    
    for(std::map<int, int>::iterator it = f->links.begin(); it != f->links.end(); it++)
    {
        Factory* curr = getFactory(it->first);
        if(f->getSoldierCount() > curr->getSoldierCount() + curr->getProduction() * it->second
            && it->second < closestDis && !curr->isMine() && curr->getProduction() > 0)
        {
            closestID = it->first;
            closestDis = it->second;
        }
    }
    return closestID;
}

int Graph::closestFriendlyUnproductive(int id)
{
    Factory* f = getFactory(id);
    int closestID = -1;
    int closestDis = 100000;
    
    for(std::map<int, int>::iterator it = f->links.begin(); it != f->links.end(); it++)
    {
        Factory* curr = getFactory(it->first);
        if(f->getSoldierCount() > curr->getSoldierCount() + curr->getProduction() * it->second
            && it->second < closestDis && curr->isMine() && curr->getProduction() == 0)
        {
            closestID = it->first;
            closestDis = it->second;
        }
    }
    return closestID;
}

int Graph::closestFriendInNeed(int id, TroopManager* tm)
{
    Factory* f = getFactory(id);
    int closestID = -1;
    int closestDis = 100000;
    
    for(std::map<int, int>::iterator it = f->links.begin(); it != f->links.end(); it++)
    {
        Factory* curr = getFactory(it->first);
        if(curr->getSoldierCount() < tm->amountOfEnemiesComing(curr)
            && it->second < closestDis && curr->isMine())
        {
            closestID = it->first;
            closestDis = it->second;
        }
    }
    return closestID;
}

int Graph::weakestProductiveFactory()
{
    int weakestId = -1;
    int lessForces = 100000;
    for(std::vector<Factory*>::iterator it = factories.begin(); it < factories.end(); it++)
    {
        if((*it)->getSoldierCount() < lessForces && !((*it)->isMine()
                && (*it)->getProduction() > 0))
        {
            weakestId = (*it)->ID;
            lessForces = (*it)->getSoldierCount();
        }
    }
    return weakestId; 
}

int Graph::closestAvailableFactory(int factoryID)
{
    Factory* f = getFactory(factoryID);
    int closestID = -1;
    int closestDis = 100000;
    
    for(std::map<int, int>::iterator it = f->links.begin(); it != f->links.end(); it++)
    {
        Factory* curr = getFactory(it->first);
        if(curr->getSoldierCount() > f->getSoldierCount() + f->getProduction() * it->second
            && it->second < closestDis && curr->isMine())
        {
            closestID = it->first;
            closestDis = it->second;
        }
    }
    return closestID;
}

int Graph::biggestEnemyFactory()
{
    int strongest = -1;
    int mostForces = -1;
    for(std::vector<Factory*>::iterator it = factories.begin(); it < factories.end(); it++)
    {
        if((*it)->getSoldierCount() > mostForces && (*it)->isEnemy())
        {
            strongest = (*it)->ID;
            mostForces = (*it)->getSoldierCount();
        }
    }
    return strongest;
}

int Graph::getBomberIdeal(int id)
{
    Factory* f = getFactory(id);
    int closestID = -1;
    int closestDis = 100000;
    
    for(std::map<int, int>::iterator it = f->links.begin(); it != f->links.end(); it++)
    {
        Factory* curr = getFactory(it->first);
        if(it->second < closestDis && curr->isMine())
        {
            closestID = it->first;
            closestDis = it->second;
        }
    }
    return closestID;
}



int TroopManager::amountOfEnemiesComing(Factory* target)
{
    int total = 0;
    for(std::vector<Troop*>::iterator it = troops.begin(); it < troops.end(); it++)
    {
        if((*it)->targetID == target->ID)
        {
            total+=(*it)->strength;
        }
    }
    return total;
}


class OutputManager
{
    private:
    std::string out;
    
    public:
    OutputManager();
    void wait();
    void message(std::string msg);
    void move(int src, int target, int qty);
    void bomb(int src, int target);
    void inc(int src);
    void pop();
};

OutputManager::OutputManager()
{
    out = "WAIT";
}

void OutputManager::wait()
{
    out+=";WAIT";
}

void OutputManager::message(std::string msg)
{
    out+=";MSG " + msg;
}

void OutputManager::move(int src, int target, int qty)
{
    out+=";MOVE " + std::to_string(src) + " " + std::to_string(target) + " " + std::to_string(qty);
}

void OutputManager::bomb(int src, int target)
{
    out+=";BOMB " + std::to_string(src) + " " + std::to_string(target);
}

void OutputManager::inc(int src)
{
    out+=";INC " + std::to_string(src);
}

void OutputManager::pop()
{
    std::cout << out << std::endl;
}

int main()
{
    int factoryCount; // the number of factories
    std::cin >> factoryCount; std::cin.ignore();
    int linkCount; // the number of links between factories
    std::cin >> linkCount; std::cin.ignore();
    
    int upgradeAt = 10;
    int bombs = 2;
    
    Graph g;
    
    for (int i = 0; i < linkCount; i++) {
        int factory1;
        int factory2;
        int distance;
        std::cin >> factory1 >> factory2 >> distance; std::cin.ignore();
        Factory* f1;
        Factory* f2;
        
        if(!g.contains(factory1))
        {
            f1 = new Factory(factory1);
            g.factories.push_back(f1);
        }else{
            f1 = g.getFactory(factory1);
        }
        
        if(!g.contains(factory2))
        {
            f2 = new Factory(factory2);
            g.factories.push_back(f2);
        }else{
            f2 = g.getFactory(factory2);
        }
        
        f1->links[factory2] = distance;
        f2->links[factory1] = distance;
    }
    

    while (1) {
        int entityCount; // the number of entities (e.g. factories and troops)
        std::cin >> entityCount; std::cin.ignore();
        TroopManager tm;
        OutputManager om;
        bool bombing = false;
        for (int i = 0; i < entityCount; i++) {
            int entityId;
            std::string entityType;
            int arg1;
            int arg2;
            int arg3;
            int arg4;
            int arg5;
            std::cin >> entityId >> entityType >> arg1 >> arg2 >> arg3 >> arg4 >> arg5; std::cin.ignore();
            
            if(entityType == "FACTORY")
            {
                Factory* f = g.getFactory(entityId);
                f->setOwnerShip(arg1);
                f->setSoldierCount(arg2);
                f->setProduction(arg3);
            }else if(entityType == "TROOP")
            {
                Troop* t = new Troop();
                t->ownership = arg1;
                t->sourceID = arg2;
                t->targetID = arg3;
                t->strength = arg4;
                t->arrivalTime = arg5;
                tm.troops.push_back(t);
            }else if(entityType == "BOMB")
            {
                if(arg1 == 1) bombing = true;
            }
        }
        
        for(std::vector<Factory*>::iterator it = g.factories.begin(); it < g.factories.end(); it++)
        {
            Factory* f = *it;
            
            if(f->isMine()){
				
				if(f->getSoldierCount() <= 2)continue;
				
                int attackedId  = g.closestProductiveFactory(f->ID);
					
				int solds = g.soldiersToSend(f->ID, attackedId, &tm);
					
				
				if((attackedId == -1  || f->getSoldierCount() <= solds) && f->getProduction() > 0)
				{
				    int helpedId = g.closestFriendInNeed(f->ID, &tm);
				    if(helpedId != -1)
				    {  
				        if(tm.amountOfEnemiesComing(f) < f->getSoldierCount())
		                {
		                    om.move(f->ID, helpedId, f->getProduction());
				            f->setSoldierCount(f->getSoldierCount() - f->getProduction());
		                }
			        }else{
			            helpedId = g.closestFriendlyUnproductive(f->ID);
			            if(helpedId != -1)
			            {
			                if(tm.amountOfEnemiesComing(f) < f->getSoldierCount())
			                {
			                    om.move(f->ID, helpedId, f->getProduction());
					            f->setSoldierCount(f->getSoldierCount() - f->getProduction());
			                }
			            }else{
			                if(f->getSoldierCount() - upgradeAt > tm.amountOfEnemiesComing(f) && f->getProduction() < 3)
			                {
        			            om.inc(f->ID);
                                f->setSoldierCount(f->getSoldierCount() - upgradeAt);
                                f->increaseProduction();
			                }
			            }
			        }
				}else if(f->getSoldierCount() > upgradeAt + (upgradeAt*f->getProduction())/3 
				    && f->getProduction() < 3 && attackedId == -1){
				    
                    if(tm.amountOfEnemiesComing(f) < f->getSoldierCount() - upgradeAt)
                    {
                        om.inc(f->ID);
                        f->setSoldierCount(f->getSoldierCount() - upgradeAt);
                        f->increaseProduction();
                    }
				}else{
			    	while(attackedId != -1)
				    {
    					int soldiersToSend = g.soldiersToSend(f->ID, attackedId, &tm);
    					
    					if(f->getSoldierCount() > soldiersToSend)
    					{
    					    om.move(f->ID, attackedId, soldiersToSend);
    						f->setSoldierCount(f->getSoldierCount() - soldiersToSend);
    						
    						g.getFactory(attackedId)->setSoldierCount(5000);
    					}else break;
    					
    					attackedId  = g.closestProductiveFactory(f->ID);
				    }          
				}
            }
        }
        Factory* biggestEnemy = g.getFactory(g.biggestEnemyFactory());
        if(bombs > 0 && biggestEnemy != NULL && !bombing)
        {
            if(biggestEnemy->getSoldierCount() > 10 && biggestEnemy->getProduction() == 3){
                
                int bomber = g.getBomberIdeal(biggestEnemy->ID);
                om.bomb(bomber, biggestEnemy->ID);
                bombs--;
            }   
        }
        
        om.pop();
    }
}
