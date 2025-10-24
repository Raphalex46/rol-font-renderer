#ifndef __COMMON_UTILS_SHADER_H__
#define __COMMON_UTILS_SHADER_H__

typedef unsigned int shaderId;

shaderId compileAndLinkShader(const char *vertexPath, const char *fragPath);
void useShader(shaderId Id);
void setInt(shaderId Id, const char *name, int value);
void setFloat(shaderId Id, const char *name, float value);


#endif // __COMMON_UTILS_SHADER_H__
