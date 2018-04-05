#include "CodeExecution.hpp"
#include "Debug.h"
#include "AccessFlag.h"
#include "ClassFile.h"
#include <iostream>
#include "Opcode.h"
#include "JavaClass.h"
#include "JavaHeap.h"
#include "Option.h"
#include <cassert>
#include "Descriptor.h"
#include <cmath>

JType* CodeExecution::execCode(const JavaClass* jc, CodeAttrCore&& ext) {
    for (decltype(ext.codeLength) op = 0; op < ext.codeLength; op++) {
        if (exception.hasUnhandledException()) {
            op--;
            auto* throwableObject = currentStackPop<JObject>();
            if (throwableObject == nullptr) {
                throw std::runtime_error("null pointer");
            }
            if (!hasInheritanceRelationship(throwableObject->jc, yrt.ma->loadClassIfAbsent("java/lang/Throwable"))) {
                throw std::runtime_error("it's not a throwable object");
            }

            if (handleException(jc, ext, throwableObject, op)) {
                while (!currentFrame->stack.empty()) {
                    auto* temp = currentStackPop<JType>();
                    delete temp;
                }
                currentFrame->stack.push(throwableObject);
                exception.sweepException();
            }
            else {
                return throwableObject;
            }
        }
#ifdef YVM_DEBUG_SHOW_BYTECODE
        for (int i = 0; i<frames.size(); i++) {
            std::cout << "-";
        }
        Inspector::printOpcode(ext.code, op);
#endif
        switch (ext.code[op]) {
        case op_nop: {
            // DO NOTHING :-)
        }
            break;
        case op_aconst_null: {
            JObject* obj = nullptr;
            currentFrame->stack.push(obj);
        }
            break;
        case op_iconst_m1: {
            currentFrame->stack.push(new JInt(-1));
        }
            break;
        case op_iconst_0: {
            currentFrame->stack.push(new JInt(0));
        }
            break;
        case op_iconst_1: {
            currentFrame->stack.push(new JInt(1));
        }
            break;
        case op_iconst_2: {
            currentFrame->stack.push(new JInt(2));
        }
            break;
        case op_iconst_3: {
            currentFrame->stack.push(new JInt(3));
        }
            break;
        case op_iconst_4: {
            currentFrame->stack.push(new JInt(4));
        }
            break;
        case op_iconst_5: {
            currentFrame->stack.push(new JInt(5));
        }
            break;
        case op_lconst_0: {
            currentFrame->stack.push(new JLong(0));
        }
            break;
        case op_lconst_1: {
            currentFrame->stack.push(new JLong(1));
        }
            break;
        case op_fconst_0: {
            currentFrame->stack.push(new JFloat(0.0f));
        }
            break;
        case op_fconst_1: {
            currentFrame->stack.push(new JFloat(1.0f));
        }
            break;
        case op_fconst_2: {
            currentFrame->stack.push(new JFloat(2.0f));
        }
            break;
        case op_dconst_0: {
            currentFrame->stack.push(new JDouble(0.0));
        }
            break;
        case op_dconst_1: {
            currentFrame->stack.push(new JDouble(1.0));
        }
            break;
        case op_bipush: {
            const u1 byte = consumeU1(ext.code, op);
            currentFrame->stack.push(new JInt(byte));
        }
            break;
        case op_sipush: {
            const u2 byte = consumeU2(ext.code, op);
            currentFrame->stack.push(new JInt(byte));
        }
            break;
        case op_ldc: {
            const u1 index = consumeU1(ext.code, op);
            loadConstantPoolItem2Stack(jc, static_cast<u2>(index));
        }
            break;
        case op_ldc_w: {
            const u2 index = consumeU2(ext.code, op);
            loadConstantPoolItem2Stack(jc, index);
        }
            break;
        case op_ldc2_w: {
            const u2 index = consumeU2(ext.code, op);
            if (typeid(*jc->raw.constPoolInfo[index]) == typeid(CONSTANT_Double)) {
                auto val = dynamic_cast<CONSTANT_Double*>(jc->raw.constPoolInfo[index])->val;
                JDouble* dval = new JDouble;
                dval->val = val;
                currentFrame->stack.push(dval);
            }
            else if (typeid(*jc->raw.constPoolInfo[index]) == typeid(CONSTANT_Long)) {
                auto val = dynamic_cast<CONSTANT_Long*>(jc->raw.constPoolInfo[index])->val;
                JLong* lval = new JLong;
                lval->val = val;
                currentFrame->stack.push(lval);
            }
            else {
                throw std::runtime_error("invalid symbolic reference index on constant pool");
            }
        }
            break;
        case op_iload: {
            const u1 index = consumeU1(ext.code, op);
            load2Stack<JInt>(index);
        }
            break;
        case op_lload: {
            const u1 index = consumeU1(ext.code, op);
            load2Stack<JLong>(index);
        }
            break;
        case op_fload: {
            const u1 index = consumeU1(ext.code, op);
            load2Stack<JFloat>(index);
        }
            break;
        case op_dload: {
            const u1 index = consumeU1(ext.code, op);
            load2Stack<JDouble>(index);
        }
            break;
        case op_aload: {
            const u1 index = consumeU1(ext.code, op);
            load2Stack<JRef>(index);
        }
            break;
        case op_iload_0: {
            load2Stack<JInt>(0);
        }
            break;
        case op_iload_1: {
            load2Stack<JInt>(1);
        }
            break;
        case op_iload_2: {
            load2Stack<JInt>(2);
        }
            break;
        case op_iload_3: {
            load2Stack<JInt>(3);
        }
            break;
        case op_lload_0: {
            load2Stack<JLong>(0);
        }
            break;
        case op_lload_1: {
            load2Stack<JLong>(1);
        }
            break;
        case op_lload_2: {
            load2Stack<JLong>(2);
        }
            break;
        case op_lload_3: {
            load2Stack<JLong>(3);
        }
            break;
        case op_fload_0: {
            load2Stack<JFloat>(0);
        }
            break;
        case op_fload_1: {
            load2Stack<JFloat>(1);
        }
            break;
        case op_fload_2: {
            load2Stack<JFloat>(2);
        }
            break;
        case op_fload_3: {
            load2Stack<JFloat>(3);
        }
            break;
        case op_dload_0: {
            load2Stack<JDouble>(0);
        }
            break;
        case op_dload_1: {
            load2Stack<JDouble>(1);
        }
            break;
        case op_dload_2: {
            load2Stack<JDouble>(2);
        }
            break;
        case op_dload_3: {
            load2Stack<JDouble>(3);
        }
            break;
        case op_aload_0: {
            load2Stack<JRef>(0);
        }
            break;
        case op_aload_1: {
            load2Stack<JRef>(1);
        }
            break;
        case op_aload_2: {
            load2Stack<JRef>(2);
        }
            break;
        case op_aload_3: {
            load2Stack<JRef>(3);
        }
            break;
#if __cplusplus >= 201103L
            [[fallthrough]]
#endif
        case op_saload:
        case op_caload:
        case op_baload:
        case op_iaload: {
            loadArrayItem2Stack<JInt>();
        }
            break;
        case op_laload: {
            loadArrayItem2Stack<JLong>();
        }
            break;
        case op_faload: {
            loadArrayItem2Stack<JFloat>();
        }
            break;
        case op_daload: {
            loadArrayItem2Stack<JDouble>();
        }
            break;
        case op_aaload: {
            loadArrayItem2Stack<JRef>();
        }
            break;
        case op_istore: {
            const u1 index = consumeU1(ext.code, op);
            store2Local<JInt>(index);
        }
            break;
        case op_lstore: {
            const u1 index = consumeU1(ext.code, op);
            store2Local<JLong>(index);
        }
            break;
        case op_fstore: {
            const u1 index = consumeU1(ext.code, op);
            store2Local<JFloat>(index);
        }
            break;
        case op_dstore: {
            const u1 index = consumeU1(ext.code, op);
            store2Local<JDouble>(index);
        }
            break;
        case op_astore: {
            const u1 index = consumeU1(ext.code, op);
            store2Local<JRef>(index);
        }
            break;
        case op_istore_0: {
            store2Local<JInt>(0);
        }
            break;
        case op_istore_1: {
            store2Local<JInt>(1);
        }
            break;
        case op_istore_2: {
            store2Local<JInt>(2);
        }
            break;
        case op_istore_3: {
            store2Local<JInt>(3);
        }
            break;
        case op_lstore_0: {
            store2Local<JLong>(0);
        }
            break;
        case op_lstore_1: {
            store2Local<JLong>(1);
        }
            break;
        case op_lstore_2: {
            store2Local<JLong>(2);
        }
            break;
        case op_lstore_3: {
            store2Local<JLong>(3);
        }
            break;
        case op_fstore_0: {
            store2Local<JFloat>(0);
        }
            break;
        case op_fstore_1: {
            store2Local<JFloat>(1);
        }
            break;
        case op_fstore_2: {
            store2Local<JFloat>(2);
        }
            break;
        case op_fstore_3: {
            store2Local<JFloat>(3);
        }
            break;
        case op_dstore_0: {
            store2Local<JDouble>(0);
        }
            break;
        case op_dstore_1: {
            store2Local<JDouble>(1);
        }
            break;
        case op_dstore_2: {
            store2Local<JDouble>(2);
        }
            break;
        case op_dstore_3: {
            store2Local<JDouble>(3);
        }
            break;
        case op_astore_0: {
            store2Local<JRef>(0);
        }
            break;
        case op_astore_1: {
            store2Local<JRef>(1);
        }
            break;
        case op_astore_2: {
            store2Local<JRef>(2);
        }
            break;
        case op_astore_3: {
            store2Local<JRef>(3);
        }
            break;
        case op_iastore: {
            storeArrayItem<JInt>();
        }
            break;
        case op_lastore: {
            storeArrayItem<JLong>();
        }
            break;
        case op_fastore: {
            storeArrayItem<JFloat>();
        }
            break;
        case op_dastore: {
            storeArrayItem<JDouble>();
        }
            break;
        case op_aastore: {
            storeArrayItem<JRef>();
        }
            break;
        case op_bastore: {
            JInt* value = currentStackPop<JInt>();
            value->val = static_cast<int8_t>(value->val);

            JInt* index = currentStackPop<JInt>();
            JArray* arrayref = currentStackPop<JArray>();
            if (arrayref == nullptr) {
                throw std::runtime_error("null pointer");
            }
            if (index->val > arrayref->length || index->val < 0) {
                throw std::runtime_error("array index out of bounds");
            }
            yrt.jheap->putArrayItem(*arrayref, index->val, value);

            delete index;
        }
            break;
#if __cplusplus >= 201103L
                [[fallthrough]]
#endif
        case op_sastore:
        case op_castore: {
            JInt* value = currentStackPop<JInt>();
            value->val = static_cast<int16_t>(value->val);

            JInt* index = currentStackPop<JInt>();
            JArray* arrayref = currentStackPop<JArray>();
            if (arrayref == nullptr) {
                throw std::runtime_error("null pointer");
            }
            if (index->val > arrayref->length || index->val < 0) {
                throw std::runtime_error("array index out of bounds");
            }
            yrt.jheap->putArrayItem(*arrayref, index->val, value);

            delete index;
        }
            break;
        case op_pop: {
            delete currentStackPop<JType>();
        }
            break;
        case op_pop2: {
            delete currentStackPop<JType>();
            delete currentStackPop<JType>();
        }
            break;
        case op_dup: {
            JType* value = currentStackPop<JType>();

            assert(typeid(*value)!= typeid(JLong) && typeid(*value)!= typeid(JDouble));
            currentFrame->stack.push(value);
            currentFrame->stack.push(cloneValue(value));
        }
            break;
        case op_dup_x1: {
            JType* value1 = currentStackPop<JType>();
            JType* value2 = currentStackPop<JType>();

            assert(IS_COMPUTATIONAL_TYPE_1(value1));
            assert(IS_COMPUTATIONAL_TYPE_1(value2));

            currentFrame->stack.push(cloneValue(value1));
            currentFrame->stack.push(value2);
            currentFrame->stack.push(value1);
        }
            break;
        case op_dup_x2: {
            JType* value1 = currentStackPop<JType>();
            JType* value2 = currentStackPop<JType>();
            JType* value3 = currentStackPop<JType>();

            if (IS_COMPUTATIONAL_TYPE_1(value1) && IS_COMPUTATIONAL_TYPE_1(value2) && IS_COMPUTATIONAL_TYPE_1(value3)) {
                // use structure 1
                currentFrame->stack.push(cloneValue(value1));
                currentFrame->stack.push(value3);
                currentFrame->stack.push(value2);
                currentFrame->stack.push(value1);
            }
            else if (IS_COMPUTATIONAL_TYPE_1(value1) && IS_COMPUTATIONAL_TYPE_2(value2)) {
                //use structure 2
                currentFrame->stack.push(value3);

                currentFrame->stack.push(cloneValue(value1));
                currentFrame->stack.push(value2);
                currentFrame->stack.push(value1);
            }
            else {
                SHOULD_NOT_REACH_HERE
            }
        }
            break;
        case op_dup2: {
            JType* value1 = currentStackPop<JType>();
            JType* value2 = currentStackPop<JType>();

            if (IS_COMPUTATIONAL_TYPE_1(value1) && IS_COMPUTATIONAL_TYPE_1(value2)) {
                // use structure 1
                currentFrame->stack.push(cloneValue(value2));
                currentFrame->stack.push(cloneValue(value1));
                currentFrame->stack.push(value2);
                currentFrame->stack.push(value1);
            }
            else if (IS_COMPUTATIONAL_TYPE_2(value1)) {
                //use structure 2
                currentFrame->stack.push(value2);

                currentFrame->stack.push(cloneValue(value1));
                currentFrame->stack.push(value1);
            }
            else {
                SHOULD_NOT_REACH_HERE
            }
        }
            break;
        case op_dup2_x1: {
            JType* value1 = currentStackPop<JType>();
            JType* value2 = currentStackPop<JType>();
            JType* value3 = currentStackPop<JType>();

            if (IS_COMPUTATIONAL_TYPE_1(value1) && IS_COMPUTATIONAL_TYPE_1(value2) && IS_COMPUTATIONAL_TYPE_1(value3)) {
                // use structure 1
                currentFrame->stack.push(cloneValue(value2));
                currentFrame->stack.push(cloneValue(value1));
                currentFrame->stack.push(value3);
                currentFrame->stack.push(value2);
                currentFrame->stack.push(value1);
            }
            else if (IS_COMPUTATIONAL_TYPE_2(value1) && IS_COMPUTATIONAL_TYPE_1(value2)) {
                //use structure 2
                currentFrame->stack.push(value3);

                currentFrame->stack.push(cloneValue(value1));
                currentFrame->stack.push(value2);
                currentFrame->stack.push(value1);
            }
            else {
                SHOULD_NOT_REACH_HERE
            }
        }
            break;
        case op_dup2_x2: {
            JType* value1 = currentStackPop<JType>();
            JType* value2 = currentStackPop<JType>();
            JType* value3 = currentStackPop<JType>();
            JType* value4 = currentStackPop<JType>();
            if (IS_COMPUTATIONAL_TYPE_1(value1) && IS_COMPUTATIONAL_TYPE_1(value2)
                && IS_COMPUTATIONAL_TYPE_1(value3) && IS_COMPUTATIONAL_TYPE_1(value4)) {
                // use structure 1
                currentFrame->stack.push(cloneValue(value2));
                currentFrame->stack.push(cloneValue(value1));
                currentFrame->stack.push(value4);
                currentFrame->stack.push(value3);
                currentFrame->stack.push(value2);
                currentFrame->stack.push(value1);
            }
            else if (IS_COMPUTATIONAL_TYPE_2(value1) && IS_COMPUTATIONAL_TYPE_1(value2) && IS_COMPUTATIONAL_TYPE_1(
                value3)) {
                //use structure 2
                currentFrame->stack.push(value4);

                currentFrame->stack.push(cloneValue(value1));
                currentFrame->stack.push(value4);
                currentFrame->stack.push(value2);
                currentFrame->stack.push(value1);
            }
            else {
                SHOULD_NOT_REACH_HERE
            }
        }
            break;
        case op_swap: {
            JType* value1 = currentStackPop<JType>();
            JType* value2 = currentStackPop<JType>();

            assert(IS_COMPUTATIONAL_TYPE_1(value1));
            assert(IS_COMPUTATIONAL_TYPE_1(value2));
            if (typeid(*value1) == typeid(JInt) && typeid(*value2) == typeid(JInt)) {
                std::swap(value1, value2);
            }
            else if (typeid(*value1) == typeid(JInt) && typeid(*value2) == typeid(JFloat)) {
                const int32_t temp = dynamic_cast<JInt*>(value1)->val;
                dynamic_cast<JInt*>(value1)->val = static_cast<int32_t>(dynamic_cast<JFloat*>(value2)->val);
                dynamic_cast<JFloat*>(value2)->val = static_cast<float>(temp);
            }
            else if (typeid(*value1) == typeid(JFloat) && typeid(*value2) == typeid(JInt)) {
                const float temp = dynamic_cast<JFloat*>(value1)->val;
                dynamic_cast<JFloat*>(value1)->val = static_cast<int32_t>(dynamic_cast<JInt*>(value2)->val);
                dynamic_cast<JInt*>(value2)->val = static_cast<int32_t>(temp);
            }
            else if (typeid(*value1) == typeid(JFloat) && typeid(*value2) == typeid(JFloat)) {
                std::swap(value1, value2);
            }
            else {
                SHOULD_NOT_REACH_HERE
            }
        }
            break;
        case op_iadd: {
            JInt* value2 = currentStackPop<JInt>();
            JInt* value1 = currentStackPop<JInt>();

            JInt* result = new JInt(value1->val + value2->val);
            currentFrame->stack.push(result);

            delete value1;
            delete value2;
        }
            break;
        case op_ladd: {
            JLong* value2 = currentStackPop<JLong>();
            JLong* value1 = currentStackPop<JLong>();

            JLong* result = new JLong(value1->val + value2->val);
            currentFrame->stack.push(result);

            delete value1;
            delete value2;
        }
            break;
        case op_fadd: {
            JFloat* value2 = currentStackPop<JFloat>();
            JFloat* value1 = currentStackPop<JFloat>();

            JFloat* result = new JFloat(value1->val + value2->val);
            currentFrame->stack.push(result);

            delete value1;
            delete value2;
        }
            break;
        case op_dadd: {
            JDouble* value2 = currentStackPop<JDouble>();
            JDouble* value1 = currentStackPop<JDouble>();

            JDouble* result = new JDouble(value1->val + value2->val);
            currentFrame->stack.push(result);

            delete value1;
            delete value2;
        }
            break;
        case op_isub: {
            JInt* value2 = currentStackPop<JInt>();
            JInt* value1 = currentStackPop<JInt>();

            JInt* result = new JInt(value1->val - value2->val);
            currentFrame->stack.push(result);

            delete value1;
            delete value2;
        }
            break;
        case op_lsub: {
            JLong* value2 = currentStackPop<JLong>();
            JLong* value1 = currentStackPop<JLong>();

            JLong* result = new JLong(value1->val - value2->val);
            currentFrame->stack.push(result);

            delete value1;
            delete value2;
        }
            break;
        case op_fsub: {
            JFloat* value2 = currentStackPop<JFloat>();
            JFloat* value1 = currentStackPop<JFloat>();

            JFloat* result = new JFloat(value1->val - value2->val);
            currentFrame->stack.push(result);

            delete value1;
            delete value2;
        }
            break;
        case op_dsub: {
            JDouble* value2 = currentStackPop<JDouble>();
            JDouble* value1 = currentStackPop<JDouble>();

            JDouble* result = new JDouble(value1->val - value2->val);
            currentFrame->stack.push(result);

            delete value1;
            delete value2;
        }
            break;
        case op_imul: {
            JInt* value2 = currentStackPop<JInt>();
            JInt* value1 = currentStackPop<JInt>();

            JInt* result = new JInt(value1->val * value2->val);
            currentFrame->stack.push(result);

            delete value1;
            delete value2;
        }
            break;
        case op_lmul: {
            JLong* value2 = currentStackPop<JLong>();
            JLong* value1 = currentStackPop<JLong>();

            JLong* result = new JLong(value1->val * value2->val);
            currentFrame->stack.push(result);

            delete value1;
            delete value2;
        }
            break;
        case op_fmul: {
            JFloat* value2 = currentStackPop<JFloat>();
            JFloat* value1 = currentStackPop<JFloat>();

            JFloat* result = new JFloat(value1->val * value2->val);
            currentFrame->stack.push(result);

            delete value1;
            delete value2;
        }
            break;
        case op_dmul: {
            JDouble* value2 = currentStackPop<JDouble>();
            JDouble* value1 = currentStackPop<JDouble>();

            JDouble* result = new JDouble(value1->val * value2->val);
            currentFrame->stack.push(result);

            delete value1;
            delete value2;
        }
            break;
        case op_idiv: {
            JInt* value2 = currentStackPop<JInt>();
            JInt* value1 = currentStackPop<JInt>();

            JInt* result = new JInt(value1->val / value2->val);
            currentFrame->stack.push(result);

            delete value1;
            delete value2;
        }
            break;
        case op_ldiv: {
            JLong* value2 = currentStackPop<JLong>();
            JLong* value1 = currentStackPop<JLong>();

            JLong* result = new JLong(value1->val / value2->val);
            currentFrame->stack.push(result);

            delete value1;
            delete value2;
        }
            break;
        case op_fdiv: {
            JFloat* value2 = currentStackPop<JFloat>();
            JFloat* value1 = currentStackPop<JFloat>();

            JFloat* result = new JFloat(value1->val / value2->val);
            currentFrame->stack.push(result);

            delete value1;
            delete value2;
        }
            break;
        case op_ddiv: {
            JDouble* value2 = currentStackPop<JDouble>();
            JDouble* value1 = currentStackPop<JDouble>();

            JDouble* result = new JDouble(value1->val / value2->val);
            currentFrame->stack.push(result);

            delete value1;
            delete value2;
        }
            break;
        case op_irem: {
            JInt* value2 = currentStackPop<JInt>();
            JInt* value1 = currentStackPop<JInt>();

            JInt* result = new JInt;
            result->val = value1->val - (value1->val / value2->val) * value2->val;
            currentFrame->stack.push(result);

            delete value1;
            delete value2;
        }
            break;
        case op_lrem: {
            JLong* value2 = currentStackPop<JLong>();
            JLong* value1 = currentStackPop<JLong>();

            JLong* result = new JLong;
            result->val = value1->val - (value1->val / value2->val) * value2->val;
            currentFrame->stack.push(result);

            delete value1;
            delete value2;
        }
            break;
        case op_frem: {
            JFloat* value2 = currentStackPop<JFloat>();
            JFloat* value1 = currentStackPop<JFloat>();

            JFloat* result = new JFloat;
            result->val = std::fmod(value1->val, value2->val);
            currentFrame->stack.push(result);

            delete value1;
            delete value2;
        }
            break;
        case op_drem: {
            JDouble* value2 = currentStackPop<JDouble>();
            JDouble* value1 = currentStackPop<JDouble>();

            JDouble* result = new JDouble;
            result->val = std::fmod(value1->val, value2->val);
            currentFrame->stack.push(result);

            delete value1;
            delete value2;
        }
            break;
        case op_ineg: {
            JInt* ival = currentStackPop<JInt>();
            ival->val = -ival->val;
            currentFrame->stack.push(ival);
        }
            break;
        case op_lneg: {
            JLong* lval = currentStackPop<JLong>();
            lval->val = -lval->val;
            currentFrame->stack.push(lval);
        }
            break;
        case op_fneg: {
            JFloat* fval = currentStackPop<JFloat>();
            fval->val = -fval->val;
            currentFrame->stack.push(fval);
        }
            break;
        case op_dneg: {
            JDouble* dval = currentStackPop<JDouble>();
            dval->val = -dval->val;
            currentFrame->stack.push(dval);
        }
            break;
        case op_ishl: {
            JInt* value2 = currentStackPop<JInt>();
            JInt* value1 = currentStackPop<JInt>();
            JInt* result = new JInt;
            result->val = value1->val * std::pow(2, value2->val & 0x1f);
            currentFrame->stack.push(result);

            delete value2;
            delete value1;
        }
            break;
        case op_lshl: {
            JInt* value2 = currentStackPop<JInt>();
            JLong* value1 = currentStackPop<JLong>();
            JLong* result = new JLong;
            result->val = value1->val * std::pow(2, value2->val & 0x3f);
            currentFrame->stack.push(result);

            delete value2;
            delete value1;
        }
            break;
        case op_ishr: {
            JInt* value2 = currentStackPop<JInt>();
            JInt* value1 = currentStackPop<JInt>();
            JInt* result = new JInt;
            result->val = std::floor(value1->val / std::pow(2, value2->val & 0x1f));
            currentFrame->stack.push(result);

            delete value2;
            delete value1;
        }
            break;
        case op_lshr: {
            JInt* value2 = currentStackPop<JInt>();
            JLong* value1 = currentStackPop<JLong>();
            JLong* result = new JLong;
            result->val = std::floor(value1->val / std::pow(2, value2->val & 0x3f));
            currentFrame->stack.push(result);

            delete value2;
            delete value1;
        }
            break;
        case op_iushr: {
            JInt* value2 = currentStackPop<JInt>();
            JInt* value1 = currentStackPop<JInt>();
            JInt* result = new JInt;
            if (value1->val > 0) {
                result->val = value1->val >> (value2->val & 0x1f);
            }
            else if (value1->val < 0) {
                result->val = (value1->val >> (value2->val & 0x1f)) + (2 << ~(value2->val & 0x1f));
            }
            else {
                throw std::runtime_error("0 is not handled");
            }
            currentFrame->stack.push(result);

            delete value2;
            delete value1;
        }
            break;
        case op_lushr: {
            JInt* value2 = currentStackPop<JInt>();
            JLong* value1 = currentStackPop<JLong>();
            JLong* result = new JLong;
            if (value1->val > 0) {
                result->val = value1->val >> (value2->val & 0x3f);
            }
            else if (value1->val < 0) {
                result->val = (value1->val >> (value2->val & 0x1f)) + (2L << ~(value2->val & 0x3f));
            }
            else {
                throw std::runtime_error("0 is not handled");
            }
            currentFrame->stack.push(result);

            delete value2;
            delete value1;
        }
            break;
        case op_iand: {
            JInt* value2 = currentStackPop<JInt>();
            JInt* value1 = currentStackPop<JInt>();

            JInt* result = new JInt(value1->val & value2->val);
            currentFrame->stack.push(result);

            delete value1;
            delete value2;
        }
            break;
        case op_land: {
            JLong* value2 = currentStackPop<JLong>();
            JLong* value1 = currentStackPop<JLong>();

            JLong* result = new JLong(value1->val & value2->val);
            currentFrame->stack.push(result);

            delete value1;
            delete value2;
        }
            break;
        case op_ior: {
            JInt* value2 = currentStackPop<JInt>();
            JInt* value1 = currentStackPop<JInt>();

            JInt* result = new JInt(value1->val | value2->val);
            currentFrame->stack.push(result);

            delete value1;
            delete value2;
        }
            break;
        case op_lor: {
            JLong* value2 = currentStackPop<JLong>();
            JLong* value1 = currentStackPop<JLong>();

            JLong* result = new JLong(value1->val | value2->val);
            currentFrame->stack.push(result);

            delete value1;
            delete value2;
        }
            break;
        case op_ixor: {
            JInt* value2 = currentStackPop<JInt>();
            JInt* value1 = currentStackPop<JInt>();

            JInt* result = new JInt(value1->val & value2->val);
            currentFrame->stack.push(result);

            delete value1;
            delete value2;
        }
            break;
        case op_lxor: {
            JLong* value2 = currentStackPop<JLong>();
            JLong* value1 = currentStackPop<JLong>();

            JLong* result = new JLong(value1->val ^ value2->val);
            currentFrame->stack.push(result);

            delete value1;
            delete value2;
        }
            break;
        case op_iinc: {
            const u1 index = ext.code[++op];
            const int8_t count = ext.code[++op];
            const int32_t extendedCount = count;
            if (IS_JINT(currentFrame->locals[index])) {
                dynamic_cast<JInt*>(currentFrame->locals[index])->val += extendedCount;
            }
            else if (IS_JLong(currentFrame->locals[index])) {
                dynamic_cast<JLong*>(currentFrame->locals[index])->val += extendedCount;
            }
            else if (IS_JFloat(currentFrame->locals[index])) {
                dynamic_cast<JFloat*>(currentFrame->locals[index])->val += extendedCount;
            }
            else if (IS_JDouble(currentFrame->locals[index])) {
                dynamic_cast<JDouble*>(currentFrame->locals[index])->val += extendedCount;
            }
            else {
                SHOULD_NOT_REACH_HERE
            }
        }
            break;
        case op_i2l: {
            typeCast<JInt, JLong>();
        }
            break;
        case op_i2f: {
            typeCast<JInt, JFloat>();
        }
            break;
        case op_i2d: {
            typeCast<JInt, JDouble>();
        }
            break;
        case op_l2i: {
            typeCast<JLong, JInt>();
        }
            break;
        case op_l2f: {
            typeCast<JLong, JFloat>();
        }
            break;
        case op_l2d: {
            typeCast<JLong, JDouble>();
        }
            break;
        case op_f2i: {
            typeCast<JFloat, JInt>();
        }
            break;
        case op_f2l: {
            typeCast<JFloat, JLong>();
        }
            break;
        case op_f2d: {
            typeCast<JFloat, JDouble>();
        }
            break;
        case op_d2i: {
            typeCast<JDouble, JInt>();
        }
            break;
        case op_d2l: {
            typeCast<JDouble, JLong>();
        }
            break;
        case op_d2f: {
            typeCast<JDouble, JFloat>();
        }
            break;
#if __cplusplus>=201703L
            [[fallthrough]]
#endif
        case op_i2c:
        case op_i2b: {
            auto* value = currentStackPop<JInt>();
            auto* result = new JInt;
            result->val = (int8_t)(value->val);
            currentFrame->stack.push(result);
            delete value;
        }
            break;
        case op_i2s: {
            auto* value = currentStackPop<JInt>();
            auto* result = new JInt;
            result->val = (int16_t)(value->val);
            currentFrame->stack.push(result);
            delete value;
        }
            break;
        case op_lcmp: {
            auto* value2 = currentStackPop<JLong>();
            auto* value1 = currentStackPop<JLong>();
            if (value1->val > value2->val) {
                auto* result = new JInt(1);
                currentFrame->stack.push(result);
            }
            else if (value1->val == value2->val) {
                auto* result = new JInt(0);
                currentFrame->stack.push(result);
            }
            else {
                auto* result = new JInt(-1);
                currentFrame->stack.push(result);
            }
            delete value1;
            delete value2;
        }
            break;
#if __cplusplus>=201703L
            [[fallthrough]]
#endif
        case op_fcmpg:
        case op_fcmpl: {
            auto* value2 = currentStackPop<JFloat>();
            auto* value1 = currentStackPop<JFloat>();
            if (value1->val > value2->val) {
                auto* result = new JInt(1);
                currentFrame->stack.push(result);
            }
            else if (std::abs(value1->val - value2->val) < 0.000001) {
                auto* result = new JInt(0);
                currentFrame->stack.push(result);
            }
            else {
                auto* result = new JInt(-1);
                currentFrame->stack.push(result);
            }
            delete value1;
            delete value2;
        }
            break;
#if __cplusplus>=201703L
                [[fallthrough]]
#endif
        case op_dcmpl:
        case op_dcmpg: {
            auto* value2 = currentStackPop<JDouble>();
            auto* value1 = currentStackPop<JDouble>();
            if (value1->val > value2->val) {
                auto* result = new JInt(1);
                currentFrame->stack.push(result);
            }
            else if (std::abs(value1->val - value2->val) < 0.000000000001) {
                auto* result = new JInt(0);
                currentFrame->stack.push(result);
            }
            else {
                auto* result = new JInt(-1);
                currentFrame->stack.push(result);
            }
            delete value1;
            delete value2;
        }
            break;
        case op_ifeq: {
            u4 currentOffset = op - 1;
            int16_t branchindex = consumeU2(ext.code, op);
            auto* value = currentStackPop<JInt>();
            if (value->val == 0) {
                op = currentOffset + branchindex;
            }
            delete value;
        }
            break;
        case op_ifne: {
            u4 currentOffset = op - 1;
            int16_t branchindex = consumeU2(ext.code, op);
            auto* value = currentStackPop<JInt>();
            if (value->val != 0) {
                op = currentOffset + branchindex;
            }
            delete value;
        }
            break;
        case op_iflt: {
            u4 currentOffset = op - 1;
            int16_t branchindex = consumeU2(ext.code, op);
            auto* value = currentStackPop<JInt>();
            if (value->val < 0) {
                op = currentOffset + branchindex;
            }
            delete value;
        }
            break;
        case op_ifge: {
            u4 currentOffset = op - 1;
            int16_t branchindex = consumeU2(ext.code, op);
            auto* value = currentStackPop<JInt>();
            if (value->val >= 0) {
                op = currentOffset + branchindex;
            }
            delete value;
        }
            break;
        case op_ifgt: {
            u4 currentOffset = op - 1;
            int16_t branchindex = consumeU2(ext.code, op);
            auto* value = currentStackPop<JInt>();
            if (value->val > 0) {
                op = currentOffset + branchindex;
            }
            delete value;
        }
            break;
        case op_ifle: {
            u4 currentOffset = op - 1;
            int16_t branchindex = consumeU2(ext.code, op);
            auto* value = currentStackPop<JInt>();
            if (value->val <= 0) {
                op = currentOffset + branchindex;
            }
            delete value;
        }
            break;
        case op_if_icmpeq: {
            u4 currentOffset = op - 1;
            int16_t branchindex = consumeU2(ext.code, op);
            auto* value2 = currentStackPop<JInt>();
            auto* value1 = currentStackPop<JInt>();
            if (value1->val == value2->val) {
                op = currentOffset + branchindex;
            }
            delete value1;
            delete value2;
        }
            break;
        case op_if_icmpne: {
            u4 currentOffset = op - 1;
            int16_t branchindex = consumeU2(ext.code, op);
            auto* value2 = currentStackPop<JInt>();
            auto* value1 = currentStackPop<JInt>();
            if (value1->val != value2->val) {
                op = currentOffset + branchindex;
            }
            delete value1;
            delete value2;
        }
            break;
        case op_if_icmplt: {
            u4 currentOffset = op - 1;
            int16_t branchindex = consumeU2(ext.code, op);
            auto* value2 = currentStackPop<JInt>();
            auto* value1 = currentStackPop<JInt>();
            if (value1->val < value2->val) {
                op = currentOffset + branchindex;
            }
            delete value1;
            delete value2;
        }
            break;
        case op_if_icmpge: {
            u4 currentOffset = op - 1;
            int16_t branchindex = consumeU2(ext.code, op);
            auto* value2 = currentStackPop<JInt>();
            auto* value1 = currentStackPop<JInt>();
            if (value1->val >= value2->val) {
                op = currentOffset + branchindex;
            }
            delete value1;
            delete value2;
        }
            break;
        case op_if_icmpgt: {
            u4 currentOffset = op - 1;
            int16_t branchindex = consumeU2(ext.code, op);
            auto* value2 = currentStackPop<JInt>();
            auto* value1 = currentStackPop<JInt>();
            if (value1->val > value2->val) {
                op = currentOffset + branchindex;
            }
            delete value1;
            delete value2;
        }
            break;
        case op_if_icmple: {
            u4 currentOffset = op - 1;
            int16_t branchindex = consumeU2(ext.code, op);
            auto* value2 = currentStackPop<JInt>();
            auto* value1 = currentStackPop<JInt>();
            if (value1->val <= value2->val) {
                op = currentOffset + branchindex;
            }
            delete value1;
            delete value2;
        }
            break;
        case op_if_acmpeq: {
            u4 currentOffset = op - 1;
            int16_t branchindex = consumeU2(ext.code, op);
            auto* value2 = currentStackPop<JObject>();
            auto* value1 = currentStackPop<JObject>();
            if (value1->offset == value2->offset && value1->jc == value2->jc) {
                op = currentOffset + branchindex;
            }
            delete value1;
            delete value2;
        }
            break;
        case op_if_acmpne: {
            u4 currentOffset = op - 1;
            int16_t branchindex = consumeU2(ext.code, op);
            auto* value2 = currentStackPop<JObject>();
            auto* value1 = currentStackPop<JObject>();
            if (value1->offset != value2->offset || value1->jc != value2->jc) {
                op = currentOffset + branchindex;
            }
            delete value1;
            delete value2;
        }
            break;
        case op_goto: {
            u4 currentOffset = op - 1;
            int16_t branchindex = consumeU2(ext.code, op);
            op = currentOffset + branchindex;
        }
            break;
        case op_jsr: {
            throw std::runtime_error("unsupported opcode [jsr]");
        }
            break;
        case op_ret: {
            throw std::runtime_error("unsupported opcode [ret]");
        }
            break;
        case op_tableswitch: {
            u4 currentOffset = op - 1;
            op++;
            op++;
            op++; //3 bytes padding
            int32_t defaultIndex = consumeU4(ext.code, op);
            int32_t low = consumeU4(ext.code, op);
            int32_t high = consumeU4(ext.code, op);
            std::vector<int32_t> jumpOffset;
            FOR_EACH(i, high - low + 1) {
                jumpOffset.push_back(consumeU4(ext.code, op));
            }

            auto* index = currentStackPop<JInt>();
            if (index->val < low || index->val > high) {
                op = currentOffset + defaultIndex;
            }
            else {
                op = currentOffset + jumpOffset[index->val - low];
            }
            delete index;
        }
            break;
        case op_lookupswitch: {
            u4 currentOffset = op - 1;
            op++;
            op++;
            op++; //3 bytes padding
            int32_t defaultIndex = consumeU4(ext.code, op);
            int32_t npair = consumeU4(ext.code, op);
            std::map<int32_t, int32_t> matchOffset;
            FOR_EACH(i, npair) {
                matchOffset.insert(std::make_pair(consumeU4(ext.code, op), consumeU4(ext.code, op)));
            }
            auto* key = currentStackPop<JInt>();
            auto res = matchOffset.find(key->val);
            if (res != matchOffset.end()) {
                op = currentOffset + (*res).second;
            }
            else {
                op = currentOffset + defaultIndex;
            }
            delete key;
        }
            break;
        case op_ireturn: {
            return flowReturn<JInt>();
        }
            break;
        case op_lreturn: {
            return flowReturn<JLong>();
        }
            break;
        case op_freturn: {
            return flowReturn<JFloat>();
        }
            break;
        case op_dreturn: {
            return flowReturn<JDouble>();
        }
            break;
        case op_areturn: {
            return flowReturn<JType>();
        }
            break;
        case op_return: {
            return nullptr;
        }
            break;
        case op_getstatic: {
            const u2 index = consumeU2(ext.code, op);
            auto symbolicRef = parseFieldSymbolicReference(jc, index);
            JType* field = cloneValue(getStaticField(std::get<0>(symbolicRef), std::get<1>(symbolicRef),
                                                     std::get<2>(symbolicRef)));
            currentFrame->stack.push(field);
        }
            break;
        case op_putstatic: {
            u2 index = consumeU2(ext.code, op);
            JType* value = currentStackPop<JType>();
            auto symbolicRef = parseFieldSymbolicReference(jc, index);
            putStaticField(std::get<0>(symbolicRef), std::get<1>(symbolicRef), std::get<2>(symbolicRef), value);
        }
            break;
        case op_getfield: {
            u2 index = consumeU2(ext.code, op);
            JObject* objectref = currentStackPop<JObject>();
            auto symbolicRef = parseFieldSymbolicReference(jc, index);
            JType* field = cloneValue(yrt.jheap->getObjectFieldByName(std::get<0>(symbolicRef),
                                                                      std::get<1>(symbolicRef),
                                                                      std::get<2>(symbolicRef),
                                                                      objectref, 0));
            currentFrame->stack.push(field);

            delete objectref;
        }
            break;
        case op_putfield: {
            const u2 index = consumeU2(ext.code, op);
            JType* value = currentStackPop<JType>();
            JObject* objectref = currentStackPop<JObject>();
            auto symbolicRef = parseFieldSymbolicReference(jc, index);
            yrt.jheap->putObjectFieldByName(std::get<0>(symbolicRef), std::get<1>(symbolicRef),
                                            std::get<2>(symbolicRef),
                                            objectref, value, 0);

            delete objectref;
        }
            break;
        case op_invokevirtual: {
            const u2 index = consumeU2(ext.code, op);
            assert(typeid(*jc->raw.constPoolInfo[index]) == typeid(CONSTANT_Methodref));

            auto symbolicRef = parseMethodSymbolicReference(jc, index);

            if (strcmp(std::get<1>(symbolicRef), "<init>") == 0) {
                std::runtime_error("invoking method should not be instance initialization method\n");
            }
            if (!IS_SIGNATURE_POLYMORPHIC_METHOD(std::get<0>(symbolicRef)->getClassName(), std::get<1>(symbolicRef))) {
                invokeVirtual(std::get<1>(symbolicRef), std::get<2>(symbolicRef));
            }
            else {
                //TODO:TO BE IMPLEMENTED
            }

        }
            break;
        case op_invokespecial: {
            const u2 index = consumeU2(ext.code, op);
            std::tuple<JavaClass*, const char*, const char*> symbolicRef;

            if (typeid(*jc->raw.constPoolInfo[index]) == typeid(CONSTANT_InterfaceMethodref)) {
                symbolicRef = parseInterfaceMethodSymbolicReference(jc, index);
            }
            else if (typeid(*jc->raw.constPoolInfo[index]) == typeid(CONSTANT_Methodref)) {
                symbolicRef = parseMethodSymbolicReference(jc, index);
            }
            else {
                SHOULD_NOT_REACH_HERE
            }

            // If all of the following are true, let C be the direct superclass of the
            // current class :
            JavaClass* symbolicRefClass = std::get<0>(symbolicRef);
            if (strcmp("<init>", std::get<1>(symbolicRef)) != 0) {
                if (!IS_CLASS_INTERFACE(symbolicRefClass->raw.accessFlags)) {
                    if (strcmp(symbolicRefClass->getClassName(), jc->getSuperClassName()) == 0) {
                        if (IS_CLASS_SUPER(jc->raw.accessFlags)) {
                            invokeSpecial(yrt.ma->findJavaClass(jc->getSuperClassName()), std::get<1>(symbolicRef),
                                          std::get<2>(symbolicRef));
                            break;
                        }
                    }
                }
            }
            // Otherwise let C be the symbolic reference class
            invokeSpecial(std::get<0>(symbolicRef), std::get<1>(symbolicRef), std::get<2>(symbolicRef));
        }
            break;
        case op_invokestatic: {
            // Invoke a class (static) method
            const u2 index = consumeU2(ext.code, op);

            if (typeid(*jc->raw.constPoolInfo[index]) == typeid(CONSTANT_InterfaceMethodref)) {
                auto symbolicRef = parseInterfaceMethodSymbolicReference(jc, index);
                invokeStatic(std::get<0>(symbolicRef), std::get<1>(symbolicRef), std::get<2>(symbolicRef));
            }
            else if (typeid(*jc->raw.constPoolInfo[index]) == typeid(CONSTANT_Methodref)) {
                auto symbolicRef = parseMethodSymbolicReference(jc, index);
                invokeStatic(std::get<0>(symbolicRef), std::get<1>(symbolicRef), std::get<2>(symbolicRef));
            }
            else {
                SHOULD_NOT_REACH_HERE
            }
        }
            break;
        case op_invokeinterface: {
            const u2 index = consumeU2(ext.code, op);
            ++op; // read count and discard
            ++op; //opcode padding 0;

            if (typeid(*jc->raw.constPoolInfo[index]) == typeid(CONSTANT_InterfaceMethodref)) {
                auto symbolicRef = parseInterfaceMethodSymbolicReference(jc, index);
                invokeInterface(std::get<0>(symbolicRef), std::get<1>(symbolicRef), std::get<2>(symbolicRef));
            }
        }
            break;
        case op_invokedynamic: {
            throw std::runtime_error("unsupported opcode [invokedynamic]");
        }
            break;
        case op_new: {
            const u2 index = consumeU2(ext.code, op);
            JObject* objectref = execNew(jc, index);
            currentFrame->stack.push(objectref);
        }
            break;
        case op_newarray: {
            const u1 atype = ext.code[++op];
            JInt* count = currentStackPop<JInt>();

            if (count->val < 0) {
                throw std::runtime_error("negative array size");
            }
            JArray* arrayref = yrt.jheap->createPODArray(atype, count->val);

            currentFrame->stack.push(arrayref);
            delete count;
        }
            break;
        case op_anewarray: {
            const u2 index = consumeU2(ext.code, op);
            auto symbolicRef = parseClassSymbolicReference(jc, index);
            JInt* count = currentStackPop<JInt>();

            if (count->val < 0) {
                throw std::runtime_error("negative array size");
            }
            JArray* arrayref = yrt.jheap->createObjectArray(*std::get<0>(symbolicRef), count->val);

            currentFrame->stack.push(arrayref);
            delete count;
        }
            break;
        case op_arraylength: {
            JArray* arrayref = currentStackPop<JArray>();

            if (arrayref == nullptr) {
                throw std::runtime_error("null pointer\n");
            }
            JInt* length = new JInt;
            length->val = arrayref->length;
            currentFrame->stack.push(length);

            delete arrayref;
        }
            break;
        case op_athrow: {
            auto* throwableObject = currentStackPop<JObject>();
            if (throwableObject == nullptr) {
                throw std::runtime_error("null pointer");
            }
            if (!hasInheritanceRelationship(throwableObject->jc, yrt.ma->loadClassIfAbsent("java/lang/Throwable"))) {
                throw std::runtime_error("it's not a throwable object");
            }

            if (handleException(jc, ext, throwableObject, op)) {
                while (!currentFrame->stack.empty()) {
                    auto* temp = currentStackPop<JType>();
                    delete temp;
                }
                currentFrame->stack.push(throwableObject);
            }
            else /* Exception can not handled within method handlers */ {
                exception.markExpception();
                exception.setThrowExceptionInfo(throwableObject);
                return throwableObject;
            }
        }
            break;
        case op_checkcast: {
            throw std::runtime_error("unsupported opcode [checkcast]");
        }
            break;
        case op_instanceof: {
            const u2 index = consumeU2(ext.code, op);
            auto* objectref = currentStackPop<JObject>();
            if (objectref == nullptr) {
                currentFrame->stack.push(new JInt(0));
            }
            if (checkInstanceof(jc, index, objectref)) {
                currentFrame->stack.push(new JInt(1));
            }
            else {
                currentFrame->stack.push(new JInt(0));
            }
        }
            break;
        case op_monitorenter: {
            JType* ref = currentStackPop<JType>();

            if (ref == nullptr) {
                throw std::runtime_error("null pointer");
            }

            if (!yrt.jheap->hasObjectMonitor(ref)) {
                yrt.jheap->createObjectMonitor(ref);
            }
            yrt.jheap->findObjectMonitor(ref)->enter(std::this_thread::get_id());
        }
            break;
        case op_monitorexit: {
            JType* ref = currentStackPop<JType>();

            if (ref == nullptr) {
                throw std::runtime_error("null pointer");
            }
            if (!yrt.jheap->hasObjectMonitor(ref)) {
                yrt.jheap->createObjectMonitor(ref);
            }
            yrt.jheap->findObjectMonitor(ref)->exit();

        }
            break;
        case op_wide: {
            throw std::runtime_error("unsupported opcode [wide]");
        }
            break;
        case op_multianewarray: {
            throw std::runtime_error("unsupported opcode [multianewarray]");
        }
            break;
        case op_ifnull: {
            u4 currentOffset = op - 1;
            int16_t branchIndex = consumeU2(ext.code, op);
            JObject* value = currentStackPop<JObject>();
            if (value == nullptr) {
                delete value;
                op = currentOffset + branchIndex;
            }
        }
            break;
        case op_ifnonnull: {
            u4 currentOffset = op - 1;
            int16_t branchIndex = consumeU2(ext.code, op);
            JObject* value = currentStackPop<JObject>();
            if (value != nullptr) {
                delete value;
                op = currentOffset + branchIndex;
            }
        }
            break;
        case op_goto_w: {
            u4 currentOffset = op - 1;
            int32_t branchIndex = consumeU4(ext.code, op);
            op = currentOffset + branchIndex;
        }
            break;
        case op_jsr_w: {
            throw std::runtime_error("unsupported opcode [jsr_w]");
        }
            break;
        case op_breakpoint:
        case op_impdep1:
        case op_impdep2: {
            // Reserved opcodde
            std::cerr << "Are you a dot.class hacker? Or you were entered a strange region.";
            std::exit(EXIT_FAILURE);
        }
            break;
        default:
            std::cerr << "The YVM can not recognize this opcode. Bytecode file was be corrupted.";
            std::exit(EXIT_FAILURE);
        }
    }
    return nullptr;
}

/**
 * \brief This function does "ldc" opcode
 * \param jc type of JavaClass, which indicate where to resolve constant pool index
 * \param index constant pool index 
 */
void CodeExecution::loadConstantPoolItem2Stack(const JavaClass* jc, u2 index) {
    if (typeid(*jc->raw.constPoolInfo[index]) == typeid(CONSTANT_Integer)) {
        auto val = dynamic_cast<CONSTANT_Integer*>(jc->raw.constPoolInfo[index])->val;
        JInt* ival = new JInt;
        ival->val = val;
        currentFrame->stack.push(ival);
    }
    else if (typeid(*jc->raw.constPoolInfo[index]) == typeid(CONSTANT_Float)) {
        auto val = dynamic_cast<CONSTANT_Float*>(jc->raw.constPoolInfo[index])->val;
        JFloat* fval = new JFloat;
        fval->val = val;
        currentFrame->stack.push(fval);
    }
    else if (typeid(*jc->raw.constPoolInfo[index]) == typeid(CONSTANT_String)) {
        auto val = jc->getString(dynamic_cast<CONSTANT_String*>(jc->raw.constPoolInfo[index])->stringIndex);
        JObject* str = yrt.jheap->createObject(*yrt.ma->loadClassIfAbsent("java/lang/String"));
        JArray* value = yrt.jheap->createCharArray(val, strlen(val));
        // put string  into str's field; according the source file of java.lang.Object, we know that 
        // its first field was used to store chars
        yrt.jheap->putObjectFieldByOffset(*str, 0, value);
        currentFrame->stack.push(str);
    }
    else if (typeid(*jc->raw.constPoolInfo[index]) == typeid(CONSTANT_Class)) {
        throw std::runtime_error("unsupport region");
    }
    else if (typeid(*jc->raw.constPoolInfo[index]) == typeid(CONSTANT_MethodType)) {
        throw std::runtime_error("unsupport region");
    }
    else if (typeid(*jc->raw.constPoolInfo[index]) == typeid(CONSTANT_MethodHandle)) {
        throw std::runtime_error("unsupport region");
    }
    else {
        throw std::runtime_error("invalid symbolic reference index on constant pool");
    }
}

bool CodeExecution::handleException(const JavaClass* jc, const CodeAttrCore& ext, const JObject* objectref, u4& op) {
    FOR_EACH(i,ext.exceptionTableLength) {
        const char* catchTypeName = jc->getString(
            dynamic_cast<CONSTANT_Class*>(jc->raw.constPoolInfo[ext.exceptionTable[i].catchType])->nameIndex);

        if (hasInheritanceRelationship(yrt.ma->findJavaClass(objectref->jc->getClassName()),
                                       yrt.ma->findJavaClass(catchTypeName))
            && ext.exceptionTable[i].startPC <= op && op < ext.exceptionTable[i].endPC) {
            // start<=op<end
            // If we found a proper exception handler, set current pc as handlerPC of this exception table item;
            op = ext.exceptionTable[i].handlerPC - 1;
            return true;
        }
        if (ext.exceptionTable[i].catchType == 0) {
            op = ext.exceptionTable[i].handlerPC - 1;
            return true;
        }
    }

    return false;
}

std::tuple<JavaClass*, const char*, const char*> CodeExecution::parseFieldSymbolicReference(
    const JavaClass* jc, u2 index) {
    const char* symbolicReferenceFieldName = jc->getString(
        dynamic_cast<CONSTANT_NameAndType*>(jc->raw.constPoolInfo[
            dynamic_cast<CONSTANT_Fieldref*>(jc->raw.constPoolInfo[index])->nameAndTypeIndex])->nameIndex);

    const char* symbolicReferenceFieldDescriptor = jc->getString(
        dynamic_cast<CONSTANT_NameAndType*>(jc->raw.constPoolInfo[
            dynamic_cast<CONSTANT_Fieldref*>(jc->raw.constPoolInfo[index])->nameAndTypeIndex])->descriptorIndex);

    JavaClass* symbolicReferenceClass = yrt.ma->loadClassIfAbsent(jc->getString(
        dynamic_cast<CONSTANT_Class*>(jc->raw.constPoolInfo[
            dynamic_cast<CONSTANT_Fieldref*>(jc->raw.constPoolInfo[index])->
            classIndex])->nameIndex));

    yrt.ma->linkClassIfAbsent(symbolicReferenceClass->getClassName());

    return std::make_tuple(symbolicReferenceClass,
                           symbolicReferenceFieldName,
                           symbolicReferenceFieldDescriptor);
}

std::tuple<JavaClass*, const char*, const char*> CodeExecution::parseInterfaceMethodSymbolicReference(
    const JavaClass* jc, u2 index) {
    const char* symbolicReferenceInterfaceMethodName = jc->getString(
        dynamic_cast<CONSTANT_NameAndType*>(jc->raw.constPoolInfo[
            dynamic_cast<CONSTANT_InterfaceMethodref*>(jc->raw.constPoolInfo[index])->nameAndTypeIndex])->nameIndex);

    const char* symbolicReferenceInterfaceMethodDescriptor = jc->getString(
        dynamic_cast<CONSTANT_NameAndType*>(jc->raw.constPoolInfo[
            dynamic_cast<CONSTANT_InterfaceMethodref*>(jc->raw.constPoolInfo[index])->nameAndTypeIndex])->
        descriptorIndex);

    JavaClass* symbolicReferenceInterfaceMethodClass = yrt.ma->loadClassIfAbsent(jc->getString(
        dynamic_cast<CONSTANT_Class*>(jc->raw.constPoolInfo[
            dynamic_cast<CONSTANT_InterfaceMethodref*>(jc
                                                       ->raw.
                                                       constPoolInfo
                [index])->classIndex])->nameIndex));
    yrt.ma->linkClassIfAbsent(symbolicReferenceInterfaceMethodClass->getClassName());

    return std::make_tuple(symbolicReferenceInterfaceMethodClass,
                           symbolicReferenceInterfaceMethodName,
                           symbolicReferenceInterfaceMethodDescriptor);
}

std::tuple<JavaClass*, const char*, const char*> CodeExecution::parseMethodSymbolicReference(
    const JavaClass* jc, u2 index) {
    const char* symbolicReferenceMethodName = jc->getString(
        dynamic_cast<CONSTANT_NameAndType*>(jc->raw.constPoolInfo[
            dynamic_cast<CONSTANT_Methodref*>(jc->raw.constPoolInfo[index])->nameAndTypeIndex])->nameIndex);

    const char* symbolicReferenceMethodDescriptor = jc->getString(
        dynamic_cast<CONSTANT_NameAndType*>(jc->raw.constPoolInfo[
            dynamic_cast<CONSTANT_Methodref*>(jc->raw.constPoolInfo[index])->nameAndTypeIndex])->descriptorIndex);

    JavaClass* symbolicReferenceMethodClass = yrt.ma->loadClassIfAbsent(jc->getString(
        dynamic_cast<CONSTANT_Class*>(jc->raw.constPoolInfo[
            dynamic_cast<CONSTANT_Methodref*>(jc->raw.constPoolInfo[index]
            )->classIndex])->nameIndex));
    yrt.ma->linkClassIfAbsent(symbolicReferenceMethodClass->getClassName());

    return std::make_tuple(symbolicReferenceMethodClass,
                           symbolicReferenceMethodName,
                           symbolicReferenceMethodDescriptor);
}

std::tuple<JavaClass*> CodeExecution::parseClassSymbolicReference(const JavaClass* jc, u2 index) {
    const char* ref = jc->getString(dynamic_cast<CONSTANT_Class*>(jc->raw.constPoolInfo[index])->nameIndex);
    std::string str{ref};
    if (ref[0] == '[') {
        str = peelArrayComponentTypeFrom(ref);
    }
    return std::make_tuple(yrt.ma->findJavaClass(str.c_str()));
}

JType* CodeExecution::getStaticField(JavaClass* parsedJc, const char* fieldName, const char* fieldDescriptor) {
    yrt.ma->linkClassIfAbsent(parsedJc->getClassName());
    yrt.ma->initClassIfAbsent(*this, parsedJc->getClassName());

    FOR_EACH(i, parsedJc->raw.fieldsCount) {
        if (IS_FIELD_STATIC(parsedJc->raw.fields[i].accessFlags)) {
            const char* n = parsedJc->getString(parsedJc->raw.fields[i].nameIndex);
            const char* d = parsedJc->getString(parsedJc->raw.fields[i].descriptorIndex);
            if (strcmp(n, fieldName) == 0 && strcmp(d, fieldDescriptor) == 0) {
                return parsedJc->sfield.find(i)->second;
            }
        }
    }
    if (parsedJc->raw.superClass != 0) {
        return getStaticField(yrt.ma->findJavaClass(parsedJc->getSuperClassName()), fieldName, fieldDescriptor);
    }
    return nullptr;
}

void CodeExecution::putStaticField(JavaClass* parsedJc, const char* fieldName, const char* fieldDescriptor,
                                   JType* value) {
    yrt.ma->linkClassIfAbsent(parsedJc->getClassName());
    yrt.ma->initClassIfAbsent(*this, parsedJc->getClassName());

    FOR_EACH(i, parsedJc->raw.fieldsCount) {
        if (IS_FIELD_STATIC(parsedJc->raw.fields[i].accessFlags)) {
            const char* n = parsedJc->getString(parsedJc->raw.fields[i].nameIndex);
            const char* d = parsedJc->getString(parsedJc->raw.fields[i].descriptorIndex);
            if (strcmp(n, fieldName) == 0 && strcmp(d, fieldDescriptor) == 0) {
                parsedJc->sfield.find(i)->second = value;
                return;
            }
        }
    }
    if (parsedJc->raw.superClass != 0) {
        putStaticField(yrt.ma->findJavaClass(parsedJc->getSuperClassName()), fieldName, fieldDescriptor, value);
    }
}

JObject* CodeExecution::execNew(const JavaClass* jc, u2 index) {
    yrt.ma->linkClassIfAbsent(const_cast<JavaClass*>(jc)->getClassName());
    yrt.ma->initClassIfAbsent(*this, const_cast<JavaClass*>(jc)->getClassName());

    if (typeid(*jc->raw.constPoolInfo[index]) != typeid(CONSTANT_Class)) {
        throw std::runtime_error("operand index of new is not a class or interface\n");
    }
    char* className = (char *)jc->getString(dynamic_cast<CONSTANT_Class*>(jc->raw.constPoolInfo[index])->nameIndex);
    JavaClass* newClass = yrt.ma->loadClassIfAbsent(className);
    return yrt.jheap->createObject(*newClass);
}

CodeAttrCore CodeExecution::getCodeAttrCore(const MethodInfo* m) {
    CodeAttrCore ext{};
    if (!m) {
        return ext;
    }

    FOR_EACH(i, m->attributeCount) {
        if (typeid(*m->attributes[i]) == typeid(ATTR_Code)) {
            ext.code = dynamic_cast<ATTR_Code*>(m->attributes[i])->code;
            ext.codeLength = ((ATTR_Code*)m->attributes[i])->codeLength;
            ext.maxLocal = dynamic_cast<ATTR_Code*>(m->attributes[i])->maxLocals;
            ext.maxStack = dynamic_cast<ATTR_Code*>(m->attributes[i])->maxStack;
            ext.exceptionTableLength = dynamic_cast<ATTR_Code*>(m->attributes[i])->exceptionTableLength;
            ext.exceptionTable = dynamic_cast<ATTR_Code*>(m->attributes[i])->exceptionTable;
            ext.valid = true;
            break;
        }
    }
    return ext;
}

bool CodeExecution::checkInstanceof(const JavaClass* jc, u2 index, JType* objectref) {
    std::string TclassName = (char*)dynamic_cast<CONSTANT_Utf8*>(jc->raw.constPoolInfo[
        dynamic_cast<CONSTANT_Class*>(jc->raw.constPoolInfo[index])->nameIndex])->bytes;
    constexpr short TYPE_ARRAY = 1;
    constexpr short TYPE_CLASS = 2;
    constexpr short TYPE_INTERFACE = 3;

    short tType = 0;
    if (TclassName.find('[') != std::string::npos) {
        tType = TYPE_ARRAY;
    }
    else {
        if (IS_CLASS_INTERFACE(yrt.ma->findJavaClass(TclassName.c_str())->raw.accessFlags)) {
            tType = TYPE_INTERFACE;
        }
        else {
            tType = TYPE_CLASS;
        }
    }

    if (typeid(objectref) == typeid(JObject)) {
        if (!IS_CLASS_INTERFACE(dynamic_cast<JObject*>(objectref)->jc->raw.accessFlags)) {
            // If it's an ordinary class
            if (tType == TYPE_CLASS) {
                if (strcmp(yrt.ma->findJavaClass(dynamic_cast<JObject*>(objectref)->jc->getClassName())->getClassName(),
                           TclassName.c_str()) == 0 ||
                    strcmp(
                        yrt.ma->findJavaClass(dynamic_cast<JObject*>(objectref)->jc->getSuperClassName())->
                            getClassName(),
                        TclassName.c_str()) == 0) {
                    return true;
                }
            }
            else if (tType == TYPE_INTERFACE) {
                auto&& interfaceIdxs = dynamic_cast<JObject*>(objectref)->jc->getInterfacesIndex();
                FOR_EACH(i,interfaceIdxs.size()) {
                    auto* interfaceName =
                        dynamic_cast<JObject*>(objectref)->jc->getString(
                            dynamic_cast<CONSTANT_Class*>(dynamic_cast<JObject*>(
                                objectref)->jc->raw.constPoolInfo[interfaceIdxs[i
                            ]])->nameIndex);
                    if (strcmp(interfaceName, TclassName.c_str()) == 0) {
                        return true;
                    }
                }
            }
            else {
                SHOULD_NOT_REACH_HERE
            }
        }
        else {
            // Otherwise, it's an interface class 
            if (tType == TYPE_CLASS) {
                if (strcmp(TclassName.c_str(), "java/lang/Object") == 0) {
                    return true;
                }
            }
            else if (tType == TYPE_INTERFACE) {
                if (strcmp(TclassName.c_str(), dynamic_cast<JObject*>(objectref)->jc->getClassName()) == 0
                    || strcmp(TclassName.c_str(), dynamic_cast<JObject*>(objectref)->jc->getSuperClassName()) == 0) {
                    return true;
                }
            }
            else {
                SHOULD_NOT_REACH_HERE
            }
        }
    }
    else if (typeid(objectref) == typeid(JArray)) {
        if (tType == TYPE_CLASS) {
            if (strcmp(TclassName.c_str(), "java/lang/Object") == 0) {
                return true;
            }
        }
        else if (tType == TYPE_INTERFACE) {
            auto* firstComponent = dynamic_cast<JObject*>(yrt.jheap->getArrayItem(*dynamic_cast<JArray*>(objectref), 0)
            );
            auto&& interfaceIdxs = firstComponent->jc->getInterfacesIndex();
            FOR_EACH(i,interfaceIdxs.size()) {
                if (strcmp(firstComponent->jc->getString(
                               dynamic_cast<CONSTANT_Class*>(firstComponent->jc->raw.constPoolInfo[
                                   interfaceIdxs[i]])->nameIndex),
                           TclassName.c_str()) == 0) {
                    return true;
                }
            }
        }
        else if (tType == TYPE_ARRAY) {
            throw std::runtime_error("to be implement\n");
        }
        else {
            SHOULD_NOT_REACH_HERE
        }
    }
    else {
        SHOULD_NOT_REACH_HERE
    }

}

std::pair<MethodInfo *, const JavaClass*> CodeExecution::findMethod(const JavaClass* jc, const char* methodName,
                                                                    const char* methodDescriptor) {
    // Find corresponding method at current object's class;
    FOR_EACH(i, jc->raw.methodsCount) {
        auto* methodInfo = jc->getMethod(methodName, methodDescriptor);
        if (methodInfo) {
            return std::make_pair(methodInfo, jc);
        }
    }
    // Find corresponding method at object's super class unless it's an instance of
    if (jc->raw.superClass != 0) {
        JavaClass* superClass = yrt.ma->loadClassIfAbsent(jc->getSuperClassName());
        auto methodPair = findMethod(jc, methodName, methodDescriptor);
        if (methodPair.first) {
            return methodPair;
        }
    }
    // Find corresponding method at object's all interfaces if at least one interface class existing
    if (jc->raw.interfacesCount > 0) {
        FOR_EACH(eachInterface, jc->raw.interfacesCount) {
            const auto* interfaceName = jc->getString(
                dynamic_cast<CONSTANT_Class*>(jc->raw.constPoolInfo[jc->raw.interfaces[eachInterface]])->nameIndex);
            JavaClass* interfaceClass = yrt.ma->loadClassIfAbsent(interfaceName);
            auto* methodInfo = interfaceClass->getMethod(methodName, methodDescriptor);
            if (methodInfo && (!IS_METHOD_ABSTRACT(methodInfo->accessFlags)
                && !IS_METHOD_STATIC(methodInfo->accessFlags)
                && !IS_METHOD_PRIVATE(methodInfo->accessFlags))) {
                return std::make_pair(methodInfo, interfaceClass);
            }
            if (methodInfo && (!IS_METHOD_STATIC(methodInfo->accessFlags)
                && !IS_METHOD_PRIVATE(methodInfo->accessFlags))) {
                return std::make_pair(methodInfo, interfaceClass);
            }
        }
    }
    // Otherwise, failed to find corresponding method by given method name and method descriptor
    return std::make_pair(nullptr, nullptr);
}

void CodeExecution::pushMethodArguments(Frame* frame, std::vector<int>& parameter) {
    for (int64_t i = (int64_t)parameter.size() - 1; i >= 0; i--) {
        if (parameter[i] == T_INT || parameter[i] == T_BOOLEAN ||
            parameter[i] == T_CHAR || parameter[i] == T_BYTE || parameter[i] == T_SHORT) {
            auto* v = currentStackPop<JInt>();
            frame->locals.push_front(v);
        }
        else if (parameter[i] == T_FLOAT) {
            auto* v = currentStackPop<JFloat>();
            frame->locals.push_front(v);
        }
        else if (parameter[i] == T_DOUBLE) {
            auto* v = currentStackPop<JDouble>();
            frame->locals.push_front(nullptr);
            frame->locals.push_front(v);
        }
        else if (parameter[i] == T_LONG) {
            auto* v = currentStackPop<JLong>();
            frame->locals.push_front(nullptr);
            frame->locals.push_front(v);
        }
        else if (parameter[i] == T_EXTRA_ARRAY) {
            auto* v = currentStackPop<JArray>();
            frame->locals.push_front(v);
        }
        else if (parameter[i] == T_EXTRA_OBJECT) {
            auto* v = currentStackPop<JObject>();
            frame->locals.push_front(v);
        }
        else {
            SHOULD_NOT_REACH_HERE;
        }
    }
}

JObject* CodeExecution::pushMethodThisArgument(Frame* frame) {
    auto* objectref = currentStackPop<JObject>();
    frame->locals.push_front(objectref);
    return objectref;
}

void CodeExecution::invokeByName(JavaClass* jc, const char* methodName, const char* methodDescriptor) {
    const MethodInfo* m = jc->getMethod(methodName, methodDescriptor);
    CodeAttrCore ext = getCodeAttrCore(m);
    const int returnType = std::get<0>(peelMethodParameterAndType(methodDescriptor));

    if (!ext.valid) {
#ifdef YVM_DEBUG_SHOW_EXEC_FLOW
        std::cout << "Method " << jc->getClassName() << "::" << methodName << "() not found!\n";
#endif
        return;
    }

#ifdef YVM_DEBUG_SHOW_EXEC_FLOW
    for(int i=0;i<frames.size();i++){
        std::cout << "-";
    }
    std::cout <<"Execute " << jc->getClassName() << "::" << methodName << "() " << methodDescriptor << "\n";
#endif

    // Actual method calling routine
    Frame* frame = new Frame;
    frame->locals.resize(ext.maxLocal);
    frames.push(frame);
    currentFrame = frames.top();

    JType* returnValue{};
    if (IS_METHOD_NATIVE(m->accessFlags)) {
        returnValue = cloneValue(invokeNative(jc->getClassName(), methodName, methodDescriptor));
    }
    else {
        returnValue = cloneValue(execCode(jc, std::move(ext)));
    }
    popFrame();
    if (returnType != T_EXTRA_VOID) {
        currentFrame->stack.push(returnValue);
    }
    if (exception.hasUnhandledException()) {
        exception.extendExceptionStackTrace(methodName);
        exception.printStackTrace();
        std::exit(EXIT_FAILURE);
    }
}

void CodeExecution::invokeInterface(const JavaClass* jc, const char* methodName, const char* methodDescriptor) {
    // Invoke interface method

    const auto invokingMethod = findMethod(jc, methodName, methodDescriptor);
#ifdef YVM_DEBUG_SHOW_EXEC_FLOW
    for (int i = 0; i<frames.size(); i++) {
        std::cout << "-";
    }
    std::cout << "Execute " << const_cast<JavaClass*>(invokingMethod.second)->getClassName() << "::" << methodName << "() " << methodDescriptor << "\n";
#endif

    if (invokingMethod.first == nullptr) {
        throw std::runtime_error("no such method existed");
    }

    auto parameterAndReturnType = peelMethodParameterAndType(methodDescriptor);
    const int returnType = std::get<0>(parameterAndReturnType);
    auto parameter = std::get<1>(parameterAndReturnType);
    Frame* frame = new Frame;
    pushMethodArguments(frame, parameter);
    pushMethodThisArgument(frame);

    CodeAttrCore ext = getCodeAttrCore(invokingMethod.first);
    frame->locals.resize(ext.maxLocal);
    frames.push(frame);
    this->currentFrame = frame;

    JType* returnValue{};
    if (IS_METHOD_NATIVE(invokingMethod.first->accessFlags)) {
        returnValue = cloneValue(invokeNative(const_cast<JavaClass*>(invokingMethod.second)->getClassName(), methodName,
                                              methodDescriptor));
    }
    else {
        returnValue = cloneValue(execCode(invokingMethod.second, std::move(ext)));
    }

    popFrame();
    if (returnType != T_EXTRA_VOID || exception.hasUnhandledException()) {
        currentFrame->stack.push(returnValue);
        if (exception.hasUnhandledException()) {
            exception.extendExceptionStackTrace(methodName);
        }
    }
}

void CodeExecution::invokeVirtual(const char* methodName, const char* methodDescriptor) {
    auto parameterAndReturnType = peelMethodParameterAndType(methodDescriptor);
    const int returnType = std::get<0>(parameterAndReturnType);
    auto parameter = std::get<1>(parameterAndReturnType);

    Frame* frame = new Frame;
    pushMethodArguments(frame, parameter);
    auto* objectref = pushMethodThisArgument(frame);
    frames.push(frame);
    this->currentFrame = frame;

    auto invokingMethod = findMethod(objectref->jc, methodName, methodDescriptor);
    auto ext = getCodeAttrCore(invokingMethod.first);
#ifdef YVM_DEBUG_SHOW_EXEC_FLOW
    for (int i = 0; i<frames.size(); i++) {
        std::cout << "-";
    }
    std::cout << "Execute " << const_cast<JavaClass*>(invokingMethod.second)->getClassName() << "::" << methodName << "() " << methodDescriptor << "\n";
#endif

    JType* returnValue{};
    if (invokingMethod.first) {
        if (IS_METHOD_NATIVE(invokingMethod.first->accessFlags)) {
            returnValue = cloneValue(invokeNative(const_cast<JavaClass*>(invokingMethod.second)->getClassName(),
                                                  invokingMethod.second->getString(invokingMethod.first->nameIndex),
                                                  invokingMethod.second->getString(
                                                      invokingMethod.first->descriptorIndex)));
        }
        else {

            returnValue = cloneValue(execCode(invokingMethod.second, std::move(ext)));
        }
    }
    else {
        throw std::runtime_error("can not find method to call");
    }

    popFrame();
    if (returnType != T_EXTRA_VOID || exception.hasUnhandledException()) {
        currentFrame->stack.push(returnValue);
        if (exception.hasUnhandledException()) {
            exception.extendExceptionStackTrace(methodName);
        }
    }
}

void CodeExecution::invokeSpecial(const JavaClass* jc, const char* methodName, const char* methodDescriptor) {
    //  Invoke instance method; special handling for superclass, private,
    //  and instance initialization method invocations

    auto parameterAndReturnType = peelMethodParameterAndType(methodDescriptor);
    const int returnType = std::get<0>(parameterAndReturnType);
    auto parameter = std::get<1>(parameterAndReturnType);

    Frame* frame = new Frame;
    pushMethodArguments(frame, parameter);
    auto* objectref = pushMethodThisArgument(frame);
    frames.push(frame);
    this->currentFrame = frame;

    const auto invokingMethod = findMethod(jc, methodName, methodDescriptor);
#ifdef YVM_DEBUG_SHOW_EXEC_FLOW
    for (int i = 0; i<frames.size(); i++) {
        std::cout << "-";
    }
    std::cout << "Execute " << const_cast<JavaClass*>(invokingMethod.second)->getClassName() << "::" << methodName << "() " << methodDescriptor << "\n";
#endif

    JType* returnValue{};
    if (invokingMethod.first) {
        if (IS_METHOD_NATIVE(invokingMethod.first->accessFlags)) {
            returnValue = cloneValue(invokeNative(const_cast<JavaClass*>(invokingMethod.second)->getClassName(),
                                                  invokingMethod.second->getString(invokingMethod.first->nameIndex),
                                                  invokingMethod.second->getString(
                                                      invokingMethod.first->descriptorIndex)));
        }
        else {
            auto ext = getCodeAttrCore(invokingMethod.first);
            frame->locals.resize(ext.maxLocal);
            returnValue = cloneValue(execCode(invokingMethod.second, std::move(ext)));
        }
    }
    else if (IS_CLASS_INTERFACE(jc->raw.accessFlags)) {
        JavaClass* javaLangObjectClass = yrt.ma->findJavaClass("java/lang/Object");
        MethodInfo* javaLangObjectMethod = javaLangObjectClass->getMethod(methodName, methodDescriptor);
        if (javaLangObjectMethod &&
            IS_METHOD_PUBLIC(javaLangObjectMethod->accessFlags) &&
            !IS_METHOD_STATIC(javaLangObjectMethod->accessFlags)) {
            if (IS_METHOD_NATIVE(javaLangObjectMethod->accessFlags)) {
                returnValue = cloneValue(invokeNative(javaLangObjectClass->getClassName(),
                                                      javaLangObjectClass->getString(javaLangObjectMethod->nameIndex),
                                                      javaLangObjectClass->getString(
                                                          javaLangObjectMethod->descriptorIndex)));
            }
            else {
                auto ext = getCodeAttrCore(javaLangObjectMethod);
                frame->locals.resize(ext.maxLocal);
                returnValue = cloneValue(execCode(javaLangObjectClass, std::move(ext)));
            }
        }
    }

    popFrame();
    if (returnType != T_EXTRA_VOID || exception.hasUnhandledException()) {
        currentFrame->stack.push(returnValue);
        if (exception.hasUnhandledException()) {
            exception.extendExceptionStackTrace(methodName);
        }
    }
}

void CodeExecution::invokeStatic(const JavaClass* jc, const char* methodName, const char* methodDescriptor) {
    // Get instance method name and descriptor from CONSTANT_Methodref locating by index
    // and get interface method parameter and return value descriptor
    yrt.ma->linkClassIfAbsent(const_cast<JavaClass*>(jc)->getClassName());
    yrt.ma->initClassIfAbsent(*this, const_cast<JavaClass*>(jc)->getClassName());

    const auto invokingMethod = findMethod(jc, methodName, methodDescriptor);
#ifdef YVM_DEBUG_SHOW_EXEC_FLOW
    for (int i = 0; i<frames.size(); i++) {
        std::cout << "-";
    }
    std::cout << "Execute " << const_cast<JavaClass*>(invokingMethod.second)->getClassName() << "::" << methodName << "() " << methodDescriptor << "\n";
#endif
    assert(IS_METHOD_STATIC(invokingMethod.first->accessFlags) == true);
    assert(IS_METHOD_ABSTRACT(invokingMethod.first->accessFlags) == false);
    assert(strcmp("<init>",methodName)!=0);

    Frame* frame = new Frame;
    auto ext = getCodeAttrCore(invokingMethod.first);
    frame->locals.resize(ext.maxLocal);

    auto parameterAndReturnType = peelMethodParameterAndType(methodDescriptor);
    const int returnType = std::get<0>(parameterAndReturnType);
    auto parameter = std::get<1>(parameterAndReturnType);
    pushMethodArguments(frame, parameter);

    frames.push(frame);
    this->currentFrame = frame;

    JType* returnValue{};
    if (IS_METHOD_NATIVE(invokingMethod.first->accessFlags)) {
        returnValue = cloneValue(invokeNative(const_cast<JavaClass*>(invokingMethod.second)->getClassName(), methodName,
                                              methodDescriptor));
    }
    else {
        returnValue = cloneValue(execCode(invokingMethod.second, std::move(ext)));
    }
    popFrame();

    if (returnType != T_EXTRA_VOID || exception.hasUnhandledException()) {
        currentFrame->stack.push(returnValue);
        if (exception.hasUnhandledException()) {
            exception.extendExceptionStackTrace(methodName);
        }
    }
}

JType* CodeExecution::invokeNative(const char* className, const char* methodName, const char* methodDescriptor) {
    std::string nativeMethod(className);
    nativeMethod.append(".");
    nativeMethod.append(methodName);
    nativeMethod.append(".");
    nativeMethod.append(methodDescriptor);
    if (yrt.nativeMethods.find(nativeMethod) != yrt.nativeMethods.end()) {
        return ((*yrt.nativeMethods.find(nativeMethod)).second)(&yrt);
    }
    return nullptr;
}
