#shader vertex
#version 330 core

// Index of attribute in glVertexAttribPointer
layout(location = 0) in vec4 a_Position;
layout(location = 1) in vec4 a_Color;
layout(location = 2) in vec2 a_TextureCoord;
layout(location = 3) in float a_TextureID;

uniform mat4 u_ViewProjection;

out vec4 v_Color;
out vec2 v_TextureCoord;
out float v_TextureID;

void main()
{
   gl_Position = u_ViewProjection * a_Position;
   v_Color = a_Color;
   v_TextureCoord = a_TextureCoord;
   v_TextureID = a_TextureID;
};

#shader fragment
#version 330 core

//index of attribute in glVertexAttribPointer
layout(location = 0) out vec4 o_Output;

in vec4 v_Color;
in vec2 v_TextureCoord;
in float v_TextureID;

uniform sampler2D u_Textures[32];

void main()
{
    int id = int(v_TextureID);
    o_Output = texture(u_Textures[id], v_TextureCoord) * v_Color;
};
