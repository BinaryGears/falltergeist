// Project includes
#include "../Exception.h"
#include "../Format/Int/File.h"
#include "../Format/Int/Procedure.h"
#include "../Format/Lst/File.h"
#include "../Format/Msg/File.h"
#include "../Format/Msg/Message.h"
#include "../Game/Game.h"
#include "../Game/Object.h"
#include "../Logger.h"
#include "../ResourceManager.h"
#include "../VM/ErrorException.h"
#include "../VM/HaltException.h"
#include "../VM/OpcodeFactory.h"
#include "../VM/Script.h"
#include "../VM/StackValue.h"

// Third-party includes

// stdlib
#include <ctime>
#include <memory>
#include <sstream>

namespace Falltergeist
{
    namespace VM
    {
        Script::Script(Format::Int::File *script, Game::Object *owner)
        {
            _owner = owner;
            _script = script;
            if (!_script) {
                throw Exception("Script::VM() - script is null");
            }
        }

        Script::Script(const std::string &filename, Game::Object *owner)
        {
            _owner = owner;
            _script = ResourceManager::getInstance()->intFileType(filename);
            if (!_script) {
                throw Exception("Script::VM() - script is null: " + filename);
            }
        }

        Script::~Script()
        {
        }

        std::string Script::filename()
        {
            return _script->filename();
        }

        bool Script::hasFunction(const std::string &name)
        {
            return _script->procedure(name) != nullptr;
        }

        void Script::call(const std::string &name)
        {
            _overrides = false;
            auto procedure = _script->procedure(name);
            if (!procedure) {
                return;
            }

            _programCounter = procedure->bodyOffset();
            _dataStack.push(0); // arguments counter;
            _returnStack.push(0); // return address
            Logger::debug("SCRIPT") << "CALLED: " << name << " [" << _script->filename() << "]" << std::endl;
            run();
            _dataStack.popInteger(); // remove function result
            Logger::debug("SCRIPT") << "Function ended" << std::endl;

            // reset special script arguments
            _sourceObject = _targetObject = nullptr;
            _actionUsed = _fixedParam = 0;
        }

        void Script::initialize()
        {
            if (_initialized) {
                return;
            }
            _programCounter = 0;
            run();
            _dataStack.popInteger(); // remove @start function result
        }

        void Script::run()
        {
            while (_programCounter != _script->size()) {
                if (_programCounter == 0 && _initialized) {
                    return;
                }
                auto offset = _programCounter;
                _script->setPosition(_programCounter);
                unsigned short opcode = _script->readOpcode();

                std::unique_ptr<OpcodeHandler> opcodeHandler(OpcodeFactory::createOpcode(opcode, this));
                try {
                    opcodeHandler->run();
                } catch (const HaltException &) {
                    return;
                } catch (const ErrorException &e) {
                    Logger::error("SCRIPT") << e.what() << " in [" << std::hex << opcode << "] at "
                                            << _script->filename() << ":0x" << offset << std::endl;
                    _dataStack.values()->clear();
                    _dataStack.push(0); // to end script properly
                    return;
                }
            }
        }

        std::string Script::msgMessage(int msg_file_num, int msg_num)
        {
            auto lst = ResourceManager::getInstance()->lstFileType("scripts/scripts.lst");
            auto scriptName = lst->strings()->at(msg_file_num - 1);
            auto msg = ResourceManager::getInstance()->msgFileType(
                    "text/english/dialog/" + scriptName.substr(0, scriptName.find(".int")).append(".msg"));
            if (!msg) {
                Logger::debug("SCRIPT")
                        << "Script::msgMessage(file, num) not found. file: " + std::to_string(msg_file_num) + " num: " +
                           std::to_string(msg_num) << std::endl;
                return "";
            }
            return msg->message(msg_num)->text();
        }

        std::string Script::msgSpeech(int msg_file_num, int msg_num)
        {
            auto lst = ResourceManager::getInstance()->lstFileType("scripts/scripts.lst");
            auto scriptName = lst->strings()->at(msg_file_num - 1);
            auto msg = ResourceManager::getInstance()->msgFileType(
                    "text/english/dialog/" + scriptName.substr(0, scriptName.find(".int")).append(".msg"));
            if (!msg) {
                Logger::debug("SCRIPT")
                        << "Script::msgSpeech(file, num) not found. file: " + std::to_string(msg_file_num) + " num: " +
                           std::to_string(msg_num) << std::endl;
                return "";
            }
            return msg->message(msg_num)->sound();
        }

        Format::Int::File *Script::script()
        {
            return _script;
        }

        unsigned int Script::programCounter()
        {
            return _programCounter;
        }

        void Script::setProgramCounter(unsigned int value)
        {
            if (value >= _script->size()) {
                std::stringstream ss;
                ss << "Script::setProgramCounter() - address out of range: " << std::hex << value;
                throw ErrorException(ss.str());
            }
            _programCounter = value;
        }

        Stack *Script::dataStack()
        {
            return &_dataStack;
        }

        Stack *Script::returnStack()
        {
            return &_returnStack;
        }

        std::vector<StackValue> *Script::LVARS()
        {
            return &_LVARS;
        }

        Game::Object *Script::owner()
        {
            return _owner;
        }

        bool Script::initialized()
        {
            return _initialized;
        }

        void Script::setInitialized(bool value)
        {
            _initialized = value;
        }

        size_t Script::SVARbase()
        {
            return _SVAR_base;
        }

        void Script::setSVARbase(size_t value)
        {
            _SVAR_base = value;
        }

        Script *Script::setFixedParam(int fixedParam)
        {
            this->_fixedParam = fixedParam;
            return this;
        }

        int Script::fixedParam() const
        {
            return _fixedParam;
        }

        Script *Script::setTargetObject(Game::Object *targetObject)
        {
            this->_targetObject = targetObject;
            return this;
        }

        Game::Object *Script::targetObject() const
        {
            return _targetObject;
        }

        Script *Script::setSourceObject(Game::Object *sourceObject)
        {
            this->_sourceObject = sourceObject;
            return this;
        }

        Game::Object *Script::sourceObject() const
        {
            return _sourceObject;
        }

        size_t Script::DVARbase()
        {
            return _DVAR_base;
        }

        void Script::setDVARBase(size_t Value)
        {
            _DVAR_base = Value;
        }

        bool Script::overrides()
        {
            return _overrides;
        }

        void Script::setOverrides(bool Value)
        {
            _overrides = Value;
        }

        SKILL Script::usedSkill() const
        {
            return _usedSkill;
        }

        Script *Script::setUsedSkill(SKILL skill)
        {
            _usedSkill = skill;
            return this;
        }
    }
}
