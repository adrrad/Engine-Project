
void main()
{
    CalculateStandardProperties();
    gl_Position = Renderer.mesh.MVP * vec4(v_position, 1.0);
}