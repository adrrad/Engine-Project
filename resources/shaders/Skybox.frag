in vec3 textureCoordinates;

uniform samplerCube r_u_skybox;

void main()
{    
    fragment_colour = texture(r_u_skybox, textureCoordinates);
}