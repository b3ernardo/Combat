//------------------------------------------------------BIBLIOTECAS----------------------------------------------------------

#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <math.h>

//---------------------------------------------------VARIÁVEIS GLOBAIS-------------------------------------------------------

const float FPS = 100;  
const int SCREEN_W = 960;
const int SCREEN_H = 540;
const float THETA = M_PI/4; // define o ângulo do triângulo, quanto maior o ângulo, maior a abertura do triângulo
const float RAIO_CAMPO_FORCA = 40; // todas as coordenadas do tanque dependem desse raio
const float VEL_TANQUE = 2.5; // quanto o tanque anda a cada teclada
const float PASSO_ANGULO = M_PI/90; // quanto o tanque muda o angulo a cada teclada
const int NUM_BALAS = 1; // número de balas que podem sair enquanto o tiro estiver na tela

ALLEGRO_SAMPLE *tiro = NULL;
ALLEGRO_SAMPLE *fundo = NULL;
ALLEGRO_SAMPLE *fimDeJogo = NULL;
ALLEGRO_SAMPLE_INSTANCE *fundoInstance = NULL;

//------------------------------------------------------ESTRUTURAS-----------------------------------------------------------

typedef struct Ponto {
	float x, y;
} Ponto;

typedef struct Tanque {
	Ponto centro; // coordenadas do centro 
	Ponto A, B, C; // 3 coordenadas do triângulo do tanque 
	ALLEGRO_COLOR cor;
	float vel;
	float angulo;
	float x_comp, y_comp;
	float x0, y0;
	float vel_angular;
	int pontos;
} Tanque;

typedef struct Bala {
	Ponto centro;
	float vel_bala;
	bool estado;
} Bala;

typedef struct Obstaculo {
	Ponto sup_esq, inf_dir;
	ALLEGRO_COLOR cor;
} Obstaculo;

//-----------------------------------------------FUNÇÕES DE INICIALIZAÇÃO---------------------------------------------------- 

void initTanques (Tanque *t, Tanque *t2) { // inicializa os tanques
	// pontuação do tanque 1
	t->pontos = 0;
	// pontuação do tanque 2
	t2->pontos = 0;
	
	// definindo a posição do tanque 1 na tela com as coordenadas x e y
	t->centro.x = SCREEN_W/8; 
	t->centro.y = SCREEN_H/2; 
	t->cor = al_map_rgb(0, 0, 255);

	// definindo a posição do tanque 2 na tela com as coordenadas x e y
	t2->centro.x = SCREEN_W/1.15; 
	t2->centro.y = SCREEN_H/2; 
	t2->cor = al_map_rgb(255, 0, 0); 

	//inicializando as 3 coordenadas do triangulo do tanque 1
	t->A.x = 0;
	t->A.y = -RAIO_CAMPO_FORCA;
	
	float alpha = M_PI/2 - THETA;
	float h = RAIO_CAMPO_FORCA * sin(alpha);
	float w = RAIO_CAMPO_FORCA * sin(THETA);
	
	t->B.x = -w;
	t->B.y = h;
	
	t->C.x = w;
	t->C.y = h;
	
	t->vel = 0;
	t->angulo = M_PI/2;
	t->x_comp = cos(t->angulo);
	t->y_comp = sin(t->angulo);
	
	t->vel_angular = 0;

	// inicializando as 3 coordenadas do triangulo do tanque 2	
	t2->A.x = 0;
	t2->A.y = -RAIO_CAMPO_FORCA;
	
	float alpha2 = M_PI/2 - THETA;
	float h2 = RAIO_CAMPO_FORCA * sin(alpha2);
	float w2 = RAIO_CAMPO_FORCA * sin(THETA);
	
	t2->B.x = -w2;
	t2->B.y = h2;
	
	t2->C.x = w2;
	t2->C.y = h2;
	
	t2->vel = 0;
	t2->angulo = M_PI/2;
	t2->x_comp = cos(t2->angulo);
	t2->y_comp = sin(t2->angulo);
	
	t2->vel_angular = 0;
}

void initObstaculo (Obstaculo *ret) {
	srand(time(NULL));
	ret->sup_esq.x = 360;
	ret->sup_esq.y = 200;
	ret->inf_dir.x = 600;
	ret->inf_dir.y = 340;
	ret->cor = al_map_rgb(rand()%256, rand()%256, rand()%256);
}

void initBalas (Bala balas[], Bala balas2[]) {
	int i;
	for (i = 0; i < NUM_BALAS; i++) {
		balas[i].vel_bala = 6;
		balas[i].estado = false;
	}
	for (i = 0; i < NUM_BALAS; i++) {
		balas2[i].vel_bala = 6;
		balas2[i].estado = false;
	}
}

// ----------------------------------------------FUNÇÕES DE DESENHO----------------------------------------------------------

void desenhaCenario () { // tudo que for de cenário é para ser desenhado aqui, o cenário fica abaixo de todos outros desenhos
	al_clear_to_color(al_map_rgb(0,0,0)); // limpa a tela para branco
}

void desenhaObstaculo (Obstaculo ret) {
	al_draw_filled_rectangle (ret.sup_esq.x, ret.sup_esq.y, ret.inf_dir.x, ret.inf_dir.y, ret.cor);
}

void desenhaTanques (Tanque t, Tanque t2) {
	al_draw_circle (t.centro.x, t.centro.y, RAIO_CAMPO_FORCA, t.cor, 3); // desenhando o raio do tanque 1

	al_draw_filled_triangle (t.A.x + t.centro.x, t.A.y + t.centro.y,
							 t.B.x + t.centro.x, t.B.y + t.centro.y,
							 t.C.x + t.centro.x, t.C.y + t.centro.y,
							 t.cor); // desenhando o triângulo do tanque 1

	al_draw_circle (t2.centro.x, t2.centro.y, RAIO_CAMPO_FORCA, t2.cor, 3); // desenhando o raio do tanque 1
	
	al_draw_filled_triangle (t2.A.x + t2.centro.x, t2.A.y + t2.centro.y,
							 t2.B.x + t2.centro.x, t2.B.y + t2.centro.y,
							 t2.C.x + t2.centro.x, t2.C.y + t2.centro.y,
							 t2.cor); // desenhando o triângulo do tanque 1
}

void desenhaBalas (Bala balas[], Bala balas2[]) {
	int i;
	for (i = 0; i < NUM_BALAS; i++) {
		if (balas[i].estado)
			al_draw_filled_circle(balas[i].centro.x, balas[i].centro.y, 5, al_map_rgb(255,255,255));
	}
	for (i = 0; i < NUM_BALAS; i++) {
		if (balas2[i].estado)
			al_draw_filled_circle(balas2[i].centro.x, balas2[i].centro.y, 5, al_map_rgb(255,255,255));
	}
}

//-----------------------------------------------FUNÇÕES DE MATEMÁTICA------------------------------------------------------

float dist(float x1, float y1, float x2, float y2) {
	return sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2));
}

bool colisao (Tanque t, Tanque t2) {
	if (dist(t.centro.x, t.centro.y, t2.centro.x, t2.centro.y) < RAIO_CAMPO_FORCA + RAIO_CAMPO_FORCA) {
		return true;
	} else {
		return false;
	}
}

void Rotate (Ponto *P, float Angle) {
	float x = P->x, y = P->y;
	P->x = (x * cos(Angle)) - (y * sin(Angle));
	P->y = (y * cos(Angle)) + (x * sin(Angle));
}	

//-------------------------------------------FUNÇÕES DE MOVIMENTO DO TANQUE-------------------------------------------------

void rotacionaTanques (Tanque *t, Tanque *t2) {
	if (t->vel_angular != 0) {
		Rotate(&t->A, t->vel_angular);
		Rotate(&t->B, t->vel_angular);
		Rotate(&t->C, t->vel_angular);
		
		t->angulo += t->vel_angular;
		t->x_comp = cos(t->angulo);
		t->y_comp = sin(t->angulo);
	}

	if (t2->vel_angular != 0) {
		Rotate(&t2->A, t2->vel_angular);
		Rotate(&t2->B, t2->vel_angular);
		Rotate(&t2->C, t2->vel_angular);
		
		t2->angulo += t2->vel_angular;
		t2->x_comp = cos(t2->angulo);
		t2->y_comp = sin(t2->angulo);
	}
}

void atualizaTanques (Tanque *t, Tanque *t2) {
	rotacionaTanques(t, t2);
	
	t->centro.y += t->vel * t->y_comp;
	t->centro.x += t->vel * t->x_comp;

	t2->centro.y += t2->vel * t2->y_comp;
	t2->centro.x += t2->vel * t2->x_comp;
}

//-------------------------------------------FUNÇÕES DE MOVIMENTO E COLISÃO DO TIRO-----------------------------------------------------

void atiraBala (Bala balas[], Tanque *t) {
	int i;
	for (i = 0; i < NUM_BALAS; i++) {
		if (!balas[i].estado) {
			balas[i].centro.x = t->A.x + t->centro.x;
			balas[i].centro.y = t->A.y + t->centro.y;
			balas[i].estado = true;
			t->x0 = t->x_comp;
			t->y0 = t->y_comp;
			al_play_sample(tiro, 2.0, 0.0, 2.0, ALLEGRO_PLAYMODE_ONCE, 0);
			break;
		}
	}
}

void atiraBala2 (Bala balas2[], Tanque *t2) {
	int i;
	for (i = 0; i < NUM_BALAS; i++) {
		if (!balas2[i].estado) {
			balas2[i].centro.x = t2->A.x + t2->centro.x;
			balas2[i].centro.y = t2->A.y + t2->centro.y;
			balas2[i].estado = true;
			t2->x0 = t2->x_comp;
			t2->y0 = t2->y_comp;
			al_play_sample(tiro, 2.0, 0.0, 2.0, ALLEGRO_PLAYMODE_ONCE, 0);
			break;
		}
	}
}

void atualizaBalas (Bala balas[], Bala balas2[], Tanque *t, Tanque *t2, Obstaculo *ret) {
	rotacionaTanques(t, t2);

	// tiro do tanque 1
	int i;
	for (i = 0; i < NUM_BALAS; i++) {
		if(balas[i].estado) {

			balas[i].centro.x -= balas[i].vel_bala * t->x0;
			balas[i].centro.y -= balas[i].vel_bala * t->y0;

			//colisao da bala com a tela
			if(balas[i].centro.x > SCREEN_W)
				balas[i].estado = false;
			if(balas[i].centro.x < 0)
				balas[i].estado = false;
			if(balas[i].centro.y > SCREEN_H)
				balas[i].estado = false;
			if(balas[i].centro.y < 0)
				balas[i].estado = false;
			//colisao da bala com tanque 2
			if (dist(t2->centro.x, t2->centro.y, balas[i].centro.x, balas[i].centro.y) < 5 + RAIO_CAMPO_FORCA) {
				balas[i].estado = false;
				t->pontos++;
			}
			//colisao da bala com o obstaculo
			if (balas[i].centro.x >= ret->sup_esq.x && balas[i].centro.x <= ret->inf_dir.x) {
				if (balas[i].centro.y + 5 >= ret->sup_esq.y && balas[i].centro.y - 5 <= ret->inf_dir.y) {
					balas[i].estado = false;
				}
			}
		}
	}

	// tiro do tanque 2
	for (i = 0; i < NUM_BALAS; i++) {
		if(balas2[i].estado) {

			balas2[i].centro.x -= balas2[i].vel_bala * t2->x0;
			balas2[i].centro.y -= balas2[i].vel_bala * t2->y0;

			//colisao da bala com a tela
			if(balas2[i].centro.x > SCREEN_W)
				balas2[i].estado = false;
			if(balas2[i].centro.x < 0)
				balas2[i].estado = false;
			if(balas2[i].centro.y > SCREEN_H)
				balas2[i].estado = false;
			if(balas2[i].centro.y < 0)
				balas2[i].estado = false;
			//colisao da bala com tanque 1
			if (dist(t->centro.x, t->centro.y, balas2[i].centro.x, balas2[i].centro.y) < 5 + RAIO_CAMPO_FORCA) {
				balas2[i].estado = false;
				t2->pontos++;
			}
			//colisao da bala com o obstaculo
			if (balas2[i].centro.x >= ret->sup_esq.x && balas2[i].centro.x <= ret->inf_dir.x) {
				if (balas2[i].centro.y + 5 >= ret->sup_esq.y && balas2[i].centro.y - 5 <= ret->inf_dir.y) {
					balas2[i].estado = false;
				}
			}
		}
	}
}

//-------------------------------------FUNÇÕES DE COLISÃO DO TANQUE COM OS OBSTÁCULOS---------------------------------------

void recuaTanques (Tanque *t, Tanque *t2) {
	// tanque 1
	if (colisao && t->centro.x > t2->centro.x) {
		t->centro.x += 3.5;
	}

	if (colisao && t->centro.x < t2->centro.x) {
		t->centro.x -= 3.5;
	} 

	if (colisao && t->centro.y > t2->centro.y) {
		t->centro.y += 3.5;
	}
	
	if (colisao && t->centro.y < t2->centro.y) {
		t->centro.y -= 3.5;
	}

	// tanque 2
	if (colisao && t2->centro.x > t->centro.x) {
		t2->centro.x += 3.5;
	} 

	if (colisao && t2->centro.x < t->centro.x) {
		t2->centro.x -= 3.5;
	} 

	if (colisao && t2->centro.y > t->centro.y) {
		t2->centro.y += 3.5;
	}
	
	if (colisao && t2->centro.y < t->centro.y) {
		t2->centro.y -= 3.5;
	}
}

void recuaTanquesObstaculo (Tanque *t, Tanque *t2, Obstaculo *ret) {
	// tanque 1
	// diagonais esquerdas
	if (dist(t->centro.x, t->centro.y, ret->sup_esq.x, ret->inf_dir.y) <= RAIO_CAMPO_FORCA || 
		dist(t->centro.x, t->centro.y, ret->sup_esq.x, ret->sup_esq.y) <= RAIO_CAMPO_FORCA) {
		t->centro.x -= 3.5;
	}

	// diagonais direitas
	if (dist(t->centro.x, t->centro.y, ret->inf_dir.x, ret->inf_dir.y) <= RAIO_CAMPO_FORCA || 
		dist(t->centro.x, t->centro.y, ret->inf_dir.x, ret->sup_esq.y) <= RAIO_CAMPO_FORCA) {
		t->centro.x += 3.5;
	}
	// vertical esquerda
	if (t->centro.y >= ret->sup_esq.y && t->centro.y <= ret->inf_dir.y) {
		if (t->centro.x + RAIO_CAMPO_FORCA >= ret->sup_esq.x && t->centro.x - RAIO_CAMPO_FORCA <= ret->sup_esq.x + 10) {
			t->centro.x -= 3.5;
		}
	}
	// vertical direita
	if (t->centro.y >= ret->sup_esq.y && t->centro.y <= ret->inf_dir.y) {
		if (t->centro.x - RAIO_CAMPO_FORCA <= ret->inf_dir.x && t->centro.x + RAIO_CAMPO_FORCA >= ret->inf_dir.x) {
			t->centro.x += 3.5;
		}
	}
	// horizontal superior
	if (t->centro.x >= ret->sup_esq.x && t->centro.x <= ret->inf_dir.x) {
		if (t->centro.y + RAIO_CAMPO_FORCA >= ret->sup_esq.y && t->centro.y - RAIO_CAMPO_FORCA <= ret->sup_esq.y + 10) {
			t->centro.y -= 3.5;
		}
	}
	// horizontal inferior
	if (t->centro.x >= ret->sup_esq.x && t->centro.x <= ret->inf_dir.x) {
		if (t->centro.y - RAIO_CAMPO_FORCA <= ret->inf_dir.y && t->centro.y + RAIO_CAMPO_FORCA >= ret->inf_dir.y) {
			t->centro.y += 3.5;
		}
	}

	// tanque 2
	// diagonais esquerdas
	if (dist(t2->centro.x, t2->centro.y, ret->sup_esq.x, ret->inf_dir.y) <= RAIO_CAMPO_FORCA || 
		dist(t2->centro.x, t2->centro.y, ret->sup_esq.x, ret->sup_esq.y) <= RAIO_CAMPO_FORCA) {
		t2->centro.x -= 3.5;
	}
	// diagonais direitas
	if (dist(t2->centro.x, t2->centro.y, ret->inf_dir.x, ret->inf_dir.y) <= RAIO_CAMPO_FORCA || 
		dist(t2->centro.x, t2->centro.y, ret->inf_dir.x, ret->sup_esq.y) <= RAIO_CAMPO_FORCA) {
		t2->centro.x += 3.5;
	}
	// vertical esquerda
	if (t2->centro.y >= ret->sup_esq.y && t2->centro.y <= ret->inf_dir.y) {
		if (t2->centro.x + RAIO_CAMPO_FORCA >= ret->sup_esq.x && t2->centro.x - RAIO_CAMPO_FORCA <= ret->sup_esq.x + 10) {
			t2->centro.x -= 3.5;
		}
	}

	// vertical direita
	if (t2->centro.y >= ret->sup_esq.y && t2->centro.y <= ret->inf_dir.y) {
		if (t2->centro.x - RAIO_CAMPO_FORCA <= ret->inf_dir.x && t2->centro.x + RAIO_CAMPO_FORCA >= ret->inf_dir.x) {
			t2->centro.x += 3.5;
		}
	}

	// horizontal superior
	if (t2->centro.x >= ret->sup_esq.x && t2->centro.x <= ret->inf_dir.x) {
		if (t2->centro.y + RAIO_CAMPO_FORCA >= ret->sup_esq.y && t2->centro.y - RAIO_CAMPO_FORCA <= ret->sup_esq.y + 10) {
			t2->centro.y -= 3.5;
		}
	}

	// horizontal inferior
	if (t2->centro.x >= ret->sup_esq.x && t2->centro.x <= ret->inf_dir.x) {
		if (t2->centro.y - RAIO_CAMPO_FORCA <= ret->inf_dir.y && t2->centro.y + RAIO_CAMPO_FORCA >= ret->inf_dir.y) {
			t2->centro.y += 3.5;
		}
	}
}

void recutaTanquesTela (Tanque *t, Tanque *t2) {
	// limitando a tela para o tanque 1
	if (t->centro.x + RAIO_CAMPO_FORCA > SCREEN_W) {
		t->centro.x -= 3.5; 
	} 

	if (t->centro.y + RAIO_CAMPO_FORCA > SCREEN_H) {
		t->centro.y -= 3.5; 
	}

	if (t->centro.x < RAIO_CAMPO_FORCA) {
		t->centro.x += 3.5;
	}

	if (t->centro.y < RAIO_CAMPO_FORCA) {
		t->centro.y += 3.5;
	}

	// limitando a tela para o tanque 2
	if (t2->centro.x + RAIO_CAMPO_FORCA > SCREEN_W) {
		t2->centro.x -= 3.5; 
	} 

	if (t2->centro.y + RAIO_CAMPO_FORCA > SCREEN_H) {
		t2->centro.y -= 3.5; 
	}

	if (t2->centro.x < RAIO_CAMPO_FORCA) {
		t2->centro.x += 3.5;
	}

	if (t2->centro.y < RAIO_CAMPO_FORCA) {
		t2->centro.y += 3.5;
	}
}

//-------------------------------------------------- FUNÇÃO PRINCIPAL--------------------------------------------------------

int main(int argc, char **argv){
	
	ALLEGRO_DISPLAY *display = NULL;
	ALLEGRO_EVENT_QUEUE *event_queue = NULL;
	ALLEGRO_TIMER *timer = NULL;

	// rotinas de inicializacao
	// inicializa o Allegro
	if(!al_init()) {
		fprintf(stderr, "failed to initialize allegro!\n");
		return -1;
	}
	
    // inicializa o módulo de primitivas do Allegro
    if(!al_init_primitives_addon()){
		fprintf(stderr, "failed to initialize primitives!\n");
        return -1;
    }	
   
	// cria um temporizador que incrementa uma unidade a cada 1.0/FPS segundos
    timer = al_create_timer(1.0 / FPS);
    if(!timer) {
		fprintf(stderr, "failed to create timer!\n");
		return -1;
	}
 
	// cria uma tela com dimensoes de SCREEN_W, SCREEN_H pixels
	display = al_create_display(SCREEN_W, SCREEN_H);
	if(!display) {
		fprintf(stderr, "failed to create display!\n");
		al_destroy_timer(timer);
		return -1;
	}

	// instala o teclado
	if(!al_install_keyboard()) {
		fprintf(stderr, "failed to install keyboard!\n");
		return -1;
	}

	// instala o audio
	al_install_audio();
	al_init_acodec_addon();
	al_reserve_samples(3);
	fundo = al_load_sample("soundEffect.wav");
	tiro = al_load_sample("tiro.wav");
	fimDeJogo = al_load_sample("wimusic.wav");

	fundoInstance = al_create_sample_instance(fundo);
	al_set_sample_instance_playmode(fundoInstance, ALLEGRO_PLAYMODE_LOOP);

	al_attach_sample_instance_to_mixer(fundoInstance, al_get_default_mixer());

	// inicializa o modulo allegro que carrega as fontes
	al_init_font_addon();

	// inicializa o modulo allegro que entende arquivos tff de fontes
	if(!al_init_ttf_addon()) {
		fprintf(stderr, "failed to load tff font module!\n");
		return -1;
	}
	
	// carrega o arquivo arial.ttf da fonte Arial e define que sera usado o tamanho 32 (segundo parametro)
    ALLEGRO_FONT *size_32 = al_load_font("arial.ttf", 32, 1);  

	if(size_32 == NULL) {
		fprintf(stderr, "font file does not exist or cannot be accessed!\n");
	}

 	// cria a fila de eventos
	event_queue = al_create_event_queue();
	if(!event_queue) {
		fprintf(stderr, "failed to create event_queue!\n");
		al_destroy_display(display);
		al_destroy_timer(timer);
		return -1;
	}
   
	// registra na fila os eventos de tela (ex: clicar no X na janela)
	al_register_event_source(event_queue, al_get_display_event_source(display));
	// registra na fila os eventos de tempo: quando o tempo altera de t para t+1
	al_register_event_source(event_queue, al_get_timer_event_source(timer));
	// registra na fila os eventos de teclado (ex: pressionar uma tecla)
	al_register_event_source(event_queue, al_get_keyboard_event_source());

	// cria os tanques e inicializa
	Tanque tanque_1;
	Tanque tanque_2;
	initTanques(&tanque_1, &tanque_2);
	// cria os tiros
	Bala balas[1];
	Bala balas2[1];
	// inicializa os tiros
	initBalas(balas, balas2);
	// cria o obstaculo e inicializa
	Obstaculo retangulo;
	initObstaculo(&retangulo);
	

	
	// ganhador do jogo
	int winner;
	
	// inicia o temporizador
	al_start_timer(timer);

//---------------------------------------------------------JOGANDO-----------------------------------------------------------
	
	bool playing = 1;
	while (playing) {
		// tocar musica de fundo
		al_play_sample_instance(fundoInstance);
		ALLEGRO_EVENT ev;
		// espera por um evento e o armazena na variavel de evento ev
		al_wait_for_event(event_queue, &ev);
		
		// se o tipo de evento for um evento do temporizador, ou seja, se o tempo passou de t para t+1
		if(ev.type == ALLEGRO_EVENT_TIMER) {
			
			desenhaCenario();

			desenhaObstaculo(retangulo);
			
			desenhaTanques(tanque_1, tanque_2);

			atualizaBalas(balas, balas2, &tanque_1, &tanque_2, &retangulo);

			atualizaTanques(&tanque_1, &tanque_2);

			if (colisao(tanque_1, tanque_2))
				recuaTanques(&tanque_1, &tanque_2);

			recuaTanquesObstaculo(&tanque_1, &tanque_2, &retangulo);

			recutaTanquesTela(&tanque_1, &tanque_2);
			
			// sistema de pontuacao
			// pontuacao do tanque 1
			if (tanque_1.pontos == 0) {
				al_draw_textf(size_32, al_map_rgb(0, 0, 255), 30, 30, ALLEGRO_ALIGN_LEFT, "0");
			}

			if (tanque_1.pontos == 1) {
				al_draw_textf(size_32, al_map_rgb(0, 0, 255), 30, 30, ALLEGRO_ALIGN_LEFT, "1");
			}

			if (tanque_1.pontos == 2) {
				al_draw_textf(size_32, al_map_rgb(0, 0, 255), 30, 30, ALLEGRO_ALIGN_LEFT, "2");
			}

			if (tanque_1.pontos == 3) {
				al_draw_textf(size_32, al_map_rgb(0, 0, 255), 30, 30, ALLEGRO_ALIGN_LEFT, "3");
			}

			if (tanque_1.pontos == 4) {
				al_draw_textf(size_32, al_map_rgb(0, 0, 255), 30, 30, ALLEGRO_ALIGN_LEFT, "4");
			}

			if (tanque_1.pontos == 5) {
				al_draw_textf(size_32, al_map_rgb(0, 0, 255), 30, 30, ALLEGRO_ALIGN_LEFT, "5");
				winner = 1;
				playing = 0;
			}
			
			// pontuacao do tanque 2
			if (tanque_2.pontos == 0) {
				al_draw_textf(size_32, al_map_rgb(255, 0, 0), 930, 30, ALLEGRO_ALIGN_RIGHT, "0");
			}

			if (tanque_2.pontos == 1) {
				al_draw_textf(size_32, al_map_rgb(255, 0, 0), 930, 30, ALLEGRO_ALIGN_RIGHT, "1");
			}

			if (tanque_2.pontos == 2) {
				al_draw_textf(size_32, al_map_rgb(255, 0, 0), 930, 30, ALLEGRO_ALIGN_RIGHT, "2");
			}

			if (tanque_2.pontos == 3) {
				al_draw_textf(size_32, al_map_rgb(255, 0, 0), 930, 30, ALLEGRO_ALIGN_RIGHT, "3");
			}

			if (tanque_2.pontos == 4) {
				al_draw_textf(size_32, al_map_rgb(255, 0, 0), 930, 30, ALLEGRO_ALIGN_RIGHT, "4");
			}

			if (tanque_2.pontos == 5) {
				al_draw_textf(size_32, al_map_rgb(255, 0, 0), 930, 30, ALLEGRO_ALIGN_RIGHT, "5");
				winner = 2;
				playing = 0;
			}

			// atualiza a tela (quando houver algo para mostrar)
			al_flip_display();
			
			if(al_get_timer_count(timer)%(int)FPS == 0)
				printf("\n%d segundos se passaram...", (int)(al_get_timer_count(timer)/FPS));
		}
		// se o tipo de evento for o fechamento da tela (clique no x da janela), fecha tudo
		else if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
			playing = 0;
			al_destroy_timer(timer);
			al_destroy_font(size_32);
			al_destroy_display(display);
			al_destroy_event_queue(event_queue);
		}
	
		// se o tipo de evento for um pressionar de uma tecla
		// movimentação do tanque 1 pelo teclado
		else if(ev.type == ALLEGRO_EVENT_KEY_DOWN) {
			// imprime qual tecla foi
			printf("\ncodigo tecla: %d", ev.keyboard.keycode);
			
			switch(ev.keyboard.keycode) {
				// movimento do tanque 1
				case ALLEGRO_KEY_W:
					tanque_1.vel -= VEL_TANQUE;
				break;
				
				case ALLEGRO_KEY_S:
					tanque_1.vel += VEL_TANQUE;
				break;
				
				case ALLEGRO_KEY_D:
					tanque_1.vel_angular += PASSO_ANGULO;
				break;
				
				case ALLEGRO_KEY_A:
					tanque_1.vel_angular -= PASSO_ANGULO;
				break;

				case ALLEGRO_KEY_Q:
					atiraBala(balas, &tanque_1);
				break;

				// movimento do tanque 2
				case ALLEGRO_KEY_UP:
					tanque_2.vel -= VEL_TANQUE;
				break;
				
				case ALLEGRO_KEY_DOWN:
					tanque_2.vel += VEL_TANQUE;
				break;
				
				case ALLEGRO_KEY_RIGHT:
					tanque_2.vel_angular += PASSO_ANGULO;
				break;
				
				case ALLEGRO_KEY_LEFT:
					tanque_2.vel_angular -= PASSO_ANGULO;
				break;
			}
		}
		
		else if(ev.type == ALLEGRO_EVENT_KEY_UP) {
			// imprime qual tecla foi
			printf("\ncodigo tecla: %d", ev.keyboard.keycode);
			
			switch(ev.keyboard.keycode) {
				// movimento do tanque 1
				case ALLEGRO_KEY_W:
					tanque_1.vel += VEL_TANQUE;
				break;
				
				case ALLEGRO_KEY_S:
					tanque_1.vel -= VEL_TANQUE;
				break;
				
				case ALLEGRO_KEY_D:
					tanque_1.vel_angular -= PASSO_ANGULO;
				break;
				
				case ALLEGRO_KEY_A:
					tanque_1.vel_angular += PASSO_ANGULO;
				break;

				case ALLEGRO_KEY_ENTER:
					atiraBala2(balas2, &tanque_2);
				break;

				// movimento do tanque 2
				case ALLEGRO_KEY_UP:
					tanque_2.vel += VEL_TANQUE;
				break;
				
				case ALLEGRO_KEY_DOWN:
					tanque_2.vel -= VEL_TANQUE;
				break;
				
				case ALLEGRO_KEY_RIGHT:
					tanque_2.vel_angular -= PASSO_ANGULO;
				break;
				
				case ALLEGRO_KEY_LEFT:
					tanque_2.vel_angular += PASSO_ANGULO;
				break;
			}
		}
		



		desenhaBalas(balas, balas2);

		al_flip_display();
		
	} // fim do while

//----------------------------------------------------TELA DE PONTUAÇÃO------------------------------------------------------
	al_destroy_sample(fundo);
	al_destroy_sample(tiro);

	al_play_sample(fimDeJogo, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, 0);

	char my_text[20];

	al_clear_to_color(al_map_rgb(0,0,0));

	sprintf(my_text, "P1: %d | P2: %d", tanque_1.pontos, tanque_2.pontos);	
	al_draw_text(size_32, al_map_rgb(255, 255, 255), SCREEN_W/2, SCREEN_H/3, ALLEGRO_ALIGN_CENTER, my_text);

	// abre o arquivo de historico de partidas
	FILE* file;
	file = fopen("historico.txt", "r");

	int vitorias1, vitorias2;

	// os valores dentro do arquivo recebem as variaveis vitorias1 e vitorias2
	fscanf (file, "%d %d", &vitorias1, &vitorias2);

	if (winner == 1)
		vitorias1++;

	if (winner == 2)
		vitorias2++;

	//abre o arquivo para mudar as variaveis
	file = fopen("historico.txt", "w");

	//atualiza o numero de vitorias
	fprintf (file, "%d %d", vitorias1, vitorias2);

	sprintf(my_text, "P1: %d vitorias | P2: %d vitorias", vitorias1, vitorias2);	
	al_draw_text(size_32, al_map_rgb(255, 255, 255), SCREEN_W/2, SCREEN_H/4, ALLEGRO_ALIGN_CENTER, my_text);

	fclose(file);
	al_flip_display();	
    al_rest(10);

//------------------------------------------------------DESTRUIÇÃO-----------------------------------------------------------
	
    al_destroy_sample(fimDeJogo);	
	al_destroy_timer(timer);
	al_destroy_font(size_32);
	al_destroy_display(display);
	al_destroy_event_queue(event_queue);
 
	return 0;
}
