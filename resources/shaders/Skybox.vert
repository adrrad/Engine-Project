out vec3 textureCoordinates;

void main()
{
    textureCoordinates = v_position;
    gl_Position = Renderer.camera.Projection * vec4(mat3(Renderer.camera.View) * v_position, 1.0);
}