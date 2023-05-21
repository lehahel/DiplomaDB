#pragma once

#define FIELD(type, var) \
private: \
    type var; \
public: \
    const type& Get##var() const \
    {\
        return var; \
    }\
    type& Get##var() \
    {\
        return var; \
    }\
    void Set##var(type val) \
    {\
       var = val; \
    }
