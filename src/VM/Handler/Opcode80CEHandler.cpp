// Project includes
#include "../../VM/Handler/Opcode80CEHandler.h"
#include "../../Game/CritterObject.h"
#include "../../Game/Game.h"
#include "../../PathFinding/HexagonGrid.h"
#include "../../State/Location.h"
#include "../../VM/Script.h"

// Third-party includes

// stdlib

namespace Falltergeist
{
    namespace VM
    {
        namespace Handler
        {
            Opcode80CE::Opcode80CE(VM::Script *script, std::shared_ptr<ILogger> logger) : OpcodeHandler(script)
            {
                this->logger = std::move(logger);
            }

            // TODO: handle ANIMATE_INTERRUPT
            void Opcode80CE::_run()
            {
                logger->debug()
                    << "[80CE] [=] void animate_move_obj_to_tile(void* who, int tile, int speed)"
                    << std::endl
                ;
                int speed = _script->dataStack()->popInteger();
                int tile = _script->dataStack()->popInteger();
                auto object = _script->dataStack()->popObject();

                // ANIMATE_WALK      (0)
                // ANIMATE_RUN       (1)
                // ANIMATE_INTERRUPT (16) - flag to interrupt current animation
                auto critter = dynamic_cast<Game::CritterObject *>(object);
                auto state = Game::Game::getInstance()->locationState();
                if (state) {
                    auto tileObj = state->hexagonGrid()->at(tile);
                    auto path = state->hexagonGrid()->findPath(object->hexagon(), tileObj);
                    if (path.size()) {
                        critter->stopMovement();
                        critter->setRunning((speed & 1) != 0);
                        auto queue = critter->movementQueue();
                        for (auto pathHexagon : path) {
                            queue->push_back(pathHexagon);
                        }
                    }
                }
            }
        }
    }
}
