#version 330 core 

//Kanali (in, out, uniform)
layout(location = 0) in vec2 inPos; //Pozicija tjemena
layout(location = 1) in vec2 inTex; //Boja tjemena - ovo saljemo u fragment sejder, u ovom slucaju koordinate teksture koje saljemo kao boju
out vec2 chTex;

void main() //Glavna funkcija sejdera
{
	gl_Position = vec4(inPos.x, inPos.y, 0.0, 1.0); //gl_Position je predefinisana promjenljiva za pozicije u koju stavljamo nase koordinate. Definisana je kao vec4 pa zbog toga konvertujemo
	chTex = inTex;
}