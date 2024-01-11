#version 330 core

in vec2 chTex; //Kanal iz Verteks sejdera - mora biti ISTOG IMENA I TIPA kao u vertex sejderu, koordinate teksture
out vec4 outCol; //Izlazni kanal koji ce biti zavrsna boja tjemena

uniform sampler2D uTex; //teksturna jedinica

void main() //Glavna funkcija sejdera
{
	outCol = texture(uTex, chTex);
}