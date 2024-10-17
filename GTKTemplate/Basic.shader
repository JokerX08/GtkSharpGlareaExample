#shader vertex
#version 330 core

layout (location = 0) in vec3 aPos;

uniform mat4 model;

void main()
{
   gl_Position = model * vec4(aPos, 1.0);
};


#shader fragment
#version 330 core

layout(location=0) out vec4 color;
uniform vec4 u_color;
void main()
{
   color = u_color;
};