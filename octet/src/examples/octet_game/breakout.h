namespace octet {
	class box {
		//Where is out box
		mat4t modelToWorld;

		//half the width of the box
		float halfWidth;

		//half the height of the box
		float halfHeight;

		//what colour is our box
		vec4 colour;

	public:
		box() {
		}

		void init(const vec4 &_color, float x, float y, float w, float h) {
			modelToWorld.loadIdentity();
			modelToWorld.translate(x, y, 0);
			halfWidth = w * 0.5f;
			halfHeight = h * 0.5f;
			colour = _color;
		}

		void render(color_shader &shader, mat4t &cameraToWorld) {
			mat4t modelToProjection = mat4t::build_projection_matrix(modelToWorld, cameraToWorld);

			shader.render(modelToProjection, colour);

			float vertices[] = {
				-halfWidth, -halfHeight, 0,
				halfWidth, -halfHeight, 0,
				halfWidth, halfHeight, 0,
				-halfWidth, halfHeight, 0,
			};

			glVertexAttribPointer(attribute_pos, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)vertices);
			glEnableVertexAttribArray(attribute_pos);

			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		}

		void translate(float x, float y) {
			modelToWorld.translate(x, y, 0);
		}

		void set_relative(box &rhs, float x, float y){
			modelToWorld = rhs.modelToWorld;
			modelToWorld.translate(x, y, 0);
		}

		bool collides_with(const box &rhs) const {
			float dx = rhs.modelToWorld[3][0] - modelToWorld[3][0];
			float dy = rhs.modelToWorld[3][1] - modelToWorld[3][1];

			return
				(fabsf(dx) < halfWidth + rhs.halfWidth) &&
				(fabsf(dy) < halfHeight + rhs.halfHeight)
				;
		}
	};

	class breakout : public octet::app {

		mat4t cameraToWorld;

		color_shader colour_shader_;

		texture_shader texture_shader_;

		enum state_t {
			state_begin,
			state_playing,
			state_game_over
		};

		enum {
			num_rows = 5,
			num_cols = 10,
			num_blocks = num_rows * num_cols,

			first_block = 0,
			last_block = first_block + num_blocks -1,

			game_over_sprite,

			num_sprites

		};

		int blocks_left;

		bool game_over;

		state_t state;

		int score;

		bitmap_font font;

		GLuint font_texture;

		box blocks[num_blocks];

		box border[4];

		box paddle;

		box ball;

		float ball_velocity_x;
		float ball_velocity_y;

		void simulate() {
			
			if (game_over) {
				return;
			}

			if (is_key_down(key_left)) {
				paddle.translate(-0.2f, 0);
			}
			else if (is_key_down(key_right)) {
				paddle.translate(+0.2f, 0);
			}

			if (state == state_begin) {
				ball.set_relative(paddle, 0, 0.3f);
				if (is_key_down(key_space)) {
					state = state_playing;
					ball_velocity_x = 0.1f;
					ball_velocity_y = 0.1f;
				}
			}

			else if (state == state_playing) {
				ball.translate(ball_velocity_x, ball_velocity_y);

				if (ball_velocity_y < 0 && ball.collides_with(paddle)) {
					ball_velocity_y = -ball_velocity_y;
				}

				if (ball_velocity_x > 0 && ball.collides_with(border[3])) {
					ball_velocity_x = -ball_velocity_x;
				}
				else if (ball_velocity_x < 0 && ball.collides_with(border[2])) {
					ball_velocity_x = -ball_velocity_x;
				}

				if (ball_velocity_y > 0 && ball.collides_with(border[1])) {
					ball_velocity_y = -ball_velocity_y;
				}

				if (ball.collides_with(border[0])){
					state = state_game_over;
					game_over = true;
				}

				for (int i = 0; i != 50; i++){
					if (ball_velocity_y > 0 && ball_velocity_x > 0 && ball.collides_with(blocks[i])) {
						blocks[i].translate(20,0);
						blocks_left--;
						if (ball_velocity_y < 0){
							if (ball_velocity_x < 0){
								ball_velocity_y = -ball_velocity_y;
							}
							else if (ball_velocity_x > 0) {
								ball_velocity_x = -ball_velocity_x;
							}
						}
						
						else if (ball_velocity_y > 0){
							if (ball_velocity_x < 0){
								ball_velocity_x = -ball_velocity_x;
							}
							else if (ball_velocity_x > 0) {
								ball_velocity_y = -ball_velocity_y;
							}
						}							
						//ball_velocity_x = -ball_velocity_x;
					}
					else if (ball_velocity_y < 0 && ball_velocity_x > 0 && ball.collides_with(blocks[i])) {
						blocks[i].translate(20, 0);
						blocks_left--;
						if (ball_velocity_y < 0){
							if (ball_velocity_x < 0){
								ball_velocity_y = -ball_velocity_y;
							}
							else if (ball_velocity_x > 0) {
								ball_velocity_x = -ball_velocity_x;
							}
						}

						else if (ball_velocity_y > 0){
							if (ball_velocity_x < 0){
								ball_velocity_x = -ball_velocity_x;
							}
							else if (ball_velocity_x > 0) {
								ball_velocity_y = -ball_velocity_y;
							}
						}
						//ball_velocity_x = -ball_velocity_x;
					}
					else if (ball_velocity_y < 0 && ball_velocity_x < 0 && ball.collides_with(blocks[i])) {
						blocks[i].translate(20, 0);
						blocks_left--;
						if (ball_velocity_y < 0){
							if (ball_velocity_x < 0){
								ball_velocity_y = -ball_velocity_y;
							}
							else if (ball_velocity_x > 0) {
								ball_velocity_x = -ball_velocity_x;
							}
						}

						else if (ball_velocity_y > 0){
							if (ball_velocity_x < 0){
								ball_velocity_x = -ball_velocity_x;
							}
							else if (ball_velocity_x > 0) {
								ball_velocity_y = -ball_velocity_y;
							}
						}
						//ball_velocity_x = -ball_velocity_x;
					}
					else if (ball_velocity_y > 0 && ball_velocity_x < 0 && ball.collides_with(blocks[i])) {
						blocks[i].translate(20, 0);
						blocks_left--;
						if (ball_velocity_y < 0){
							if (ball_velocity_x < 0){
								ball_velocity_y = -ball_velocity_y;
							}
							else if (ball_velocity_x > 0) {
								ball_velocity_x = -ball_velocity_x;
							}
						}

						else if (ball_velocity_y > 0){
							if (ball_velocity_x < 0){
								ball_velocity_x = -ball_velocity_x;
							}
							else if (ball_velocity_x > 0) {
								ball_velocity_y = -ball_velocity_y;
							}
						}
						//ball_velocity_x = -ball_velocity_x;
					}
				}
			}
		}

	public:

		breakout(int argc, char **argv) : app(argc, argv) {
		}
		void app_init() {

			colour_shader_.init();
			texture_shader_.init();
			cameraToWorld.loadIdentity();
			cameraToWorld.translate(0, -2.5f, 5);
			cameraToWorld.rotate(15, 1, 0,0);



			int count = 0;

			blocks_left = num_blocks;

			ball.init(vec4(1, 0, 0, 1), 0, 0, 0.25f, 0.25f);
			paddle.init(vec4(1, 1, 0, 1), 0, -4.5f, 1.0f, 0.2f);
			border[0].init(vec4(1, 1, 1, 1), 0, -4.75, 10, 0.1f);
			border[1].init(vec4(1, 1, 1, 1), 0, 4.75, 10, 0.1f);
			border[2].init(vec4(1, 1, 1, 1), -5, 0, 0.2f, 9.5);
			border[3].init(vec4(1, 1, 1, 1), 5, 0, 0.2f, 9.5);
			for (int j = 0; j != num_rows; j++) {
				for (int i = 0; i != num_cols; i++) {
					blocks[count].init(vec4(1, 1, 1, 1), ((float)i - num_cols * 0.5f) * 0.5f, 2.5f- ((float)j * 0.5f), 0.25f, 0.25f);
					count++;
				}
			}

			state = state_begin;
			score = 0;
			game_over = false;
		}

		void draw_world(int x, int y, int w, int h) {
			simulate();

			glViewport(x, y, w, h);

			glClearColor(0, 0, 0, 1);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			glEnable(GL_DEPTH_TEST);

			ball.render(colour_shader_, cameraToWorld);

			paddle.render(colour_shader_, cameraToWorld);

			for (int i = 0; i != 4; i++){
				border[i].render(colour_shader_, cameraToWorld);
			}

			for (int i = 0; i != num_blocks; i++) {
				blocks[i].render(colour_shader_, cameraToWorld);
			}
		}
	};
}