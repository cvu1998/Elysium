#shader vertex
#version 330 core

// Index of attribute in glVertexAttribPointer
layout(location = 0) in vec4 a_Position;
layout(location = 1) in vec2 a_TextureCoord;

out vec2 v_TexCoord;

uniform mat4 u_ViewProjection;

void main()
{
   gl_Position = u_ViewProjection * a_Position;
   v_TexCoord = a_TextureCoord;
};

#shader fragment
#version 330 core

//index of attribute in glVertexAttribPointer
layout(location = 0) out vec4 o_Output;

in vec2 v_TexCoord;

uniform int u_UseTexture = 1;
uniform vec4 u_Color;
uniform sampler2D u_Texture;

void main()
{
    vec4 texColor = texture(u_Texture, v_TexCoord);
    if (u_UseTexture == 1) {
        o_Output = texColor;
    }
    else {
        o_Output = u_Color;
    }
};