// Project includes
#include "../../VM/Handler/OpcodeComparisonHandler.h"
#include "../../VM/Script.h"
#include "../../VM/StackValue.h"

// Third-party includes

// stdlib

namespace Falltergeist
{
    namespace VM
    {
        namespace Handler
        {
            OpcodeComparison::OpcodeComparison(
                VM::Script *script,
                Type cmpType,
                std::shared_ptr<ILogger> logger
            ) : OpcodeHandler(script) {
                _cmpType = cmpType;
                this->logger = std::move(logger);
            }

            const char *OpcodeComparison::_cmpOpcodeName() {
                switch (_cmpType) {
                    case Type::EQUAL:
                        return "op_equal";
                    case Type::NOT_EQUAL:
                        return "op_not_equal";
                    case Type::LESS:
                        return "op_less";
                    case Type::LESS_EQUAL:
                        return "op_less_equal";
                    case Type::GREATER:
                        return "op_greater";
                    case Type::GREATER_EQUAL:
                        return "op_greater_equal";
                    default:
                        throw Exception("Invalid compare type!");
                }
            }

            void OpcodeComparison::_run()
            {
                logger->debug() << "[8033-8038] [*] " << _cmpOpcodeName() << std::endl;
                auto bValue = _script->dataStack()->pop();
                auto aValue = _script->dataStack()->pop();
                int result = 0;
                switch (aValue.type()) {
                    case StackValue::Type::INTEGER: {
                        int arg1 = aValue.integerValue();
                        switch (bValue.type()) {
                            case StackValue::Type::INTEGER: {
                                result = _compare(arg1, bValue.integerValue()); // INTEGER op INTEGER
                                break;
                            }
                            case StackValue::Type::FLOAT: {
                                result = _compare(arg1, bValue.floatValue()); // INTEGER op FLOAT
                                break;
                            }
                            case StackValue::Type::STRING: {
                                result = _compare(arg1, bValue.toInteger()); // INTEGER op STRING (parsed as int)
                                break;
                            }
                            default: {
                                _error(std::string() + _cmpOpcodeName() + ": invalid right argument type: " +
                                       bValue.typeName());
                            }
                        }
                        break;
                    }
                    case StackValue::Type::FLOAT: {
                        float arg1 = aValue.floatValue();
                        switch (bValue.type()) {
                            case StackValue::Type::INTEGER: {
                                result = _compare(arg1, bValue.integerValue()); // FLOAT op INTEGER
                                break;
                            }
                            case StackValue::Type::FLOAT: {
                                result = _compare(arg1, bValue.floatValue()); // FLOAT op FLOAT
                                break;
                            }
                            case StackValue::Type::STRING: {
                                float arg2 = 0.0;
                                try {
                                    arg2 = std::stof(bValue.stringValue());
                                }
                                catch (const std::invalid_argument &) {}
                                catch (const std::out_of_range &) {}
                                result = _compare(arg1, arg2); // FLOAT op STRING (parsed as float)
                                break;
                            }
                            default: {
                                _error(std::string() + _cmpOpcodeName() + ": invalid right argument type: " +
                                       bValue.typeName());
                            }
                        }
                        break;
                    }
                    case StackValue::Type::STRING: {
                        switch (bValue.type()) {
                            case StackValue::Type::INTEGER: {
                                result = _compare(aValue.toInteger(),
                                                  bValue.integerValue()); // STRING (as integer) op INTEGER
                                break;
                            }
                            case StackValue::Type::FLOAT: {
                                float arg1 = 0.0;
                                try {
                                    arg1 = std::stof(aValue.stringValue());
                                }
                                catch (const std::invalid_argument &) {}
                                catch (const std::out_of_range &) {}
                                result = _compare(arg1, bValue.floatValue()); // STRING (as float) op FLOAT
                                break;
                            }
                            case StackValue::Type::STRING: {
                                result = _compare(aValue.stringValue(), bValue.stringValue()); // STRING op STRING
                                break;
                            }
                            default: {
                                _error(std::string() + _cmpOpcodeName() + ": invalid right argument type: " +
                                       bValue.typeName());
                            }
                        }
                        break;
                    }
                    case StackValue::Type::OBJECT: {
                        switch (bValue.type()) {
                            case StackValue::Type::INTEGER: {
                                result = _compare((int) aValue.toBoolean(), bValue.integerValue()); // OBJECT op INTEGER
                                break;
                            }
                            case StackValue::Type::FLOAT: {
                                result = _compare((float) aValue.toBoolean(), bValue.floatValue()); // OBJECT op FLOAT
                                break;
                            }
                            case StackValue::Type::STRING: {
                                result = _compare(aValue.toString(),
                                                  bValue.stringValue()); // OBJECT op STRING - compare object name
                                break;
                            }
                            case StackValue::Type::OBJECT: {
                                result = _compare(aValue.objectValue(), bValue.objectValue()); // OBJECT op OBJECT
                                break;
                            }
                            default: {
                                _error(std::string() + _cmpOpcodeName() + ": invalid right argument type: " +
                                       bValue.typeName());
                            }
                        }
                        break;
                    }
                    default: {
                        _error(std::string() + _cmpOpcodeName() + ": invalid left argument type: " + aValue.typeName());
                    }
                }
                _script->dataStack()->push(result);
            }
        }
    }
}
