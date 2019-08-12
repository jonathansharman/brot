#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#include <chrono>
#include <cmath>
#include <thread>

int get_max_iters(float scale) {
	return static_cast<int>(1'000.0f * sqrt(sqrt(sqrt(1.0f / scale))));
}

enum class palette { fire, ice, forest };

void set_palette(sf::Shader& mset_shader, palette palette) {
	switch (palette) {
		case palette::fire:
			mset_shader.setUniform("palette", sf::Glsl::Vec3(3.0f, 2.0f, 1.0f));
			break;
		case palette::ice:
			mset_shader.setUniform("palette", sf::Glsl::Vec3(1.0f, 2.0f, 3.0f));
			break;
		case palette::forest:
			mset_shader.setUniform("palette", sf::Glsl::Vec3(1.0f, 3.0f, 1.0f));
			break;
	}
}

int main() {
	constexpr auto frame_duration = std::chrono::milliseconds{17};

	sf::Glsl::Vec2 res{800.0f, 600.0f};

	sf::RenderWindow window{
		sf::VideoMode{static_cast<unsigned>(res.x), static_cast<unsigned>(res.y)}, "Brot", sf::Style::Default};
	sf::Shader mset_shader;
	if (!mset_shader.loadFromFile("mset.frag", sf::Shader::Fragment)) {
		throw std::runtime_error{"Failed to load shader."};
	}
	mset_shader.setUniform("res", res);
	sf::Glsl::Vec2 pos{-0.5f, 0.0f};
	mset_shader.setUniform("pos", pos);
	float scale = 4.0f;
	mset_shader.setUniform("scale", scale);
	int max_iters = get_max_iters(scale);
	mset_shader.setUniform("max_iters", max_iters);
	set_palette(mset_shader, palette::ice);

	sf::RenderStates const mset_states{&mset_shader};

	sf::RectangleShape const shape{res};

	sf::Vector2f last_mouse_pos;
	bool drag = false;

	bool running = true;
	auto last_update = std::chrono::steady_clock::now();
	while (running) {
		// Update.
		sf::Event e;
		while (window.pollEvent(e)) {
			switch (e.type) {
				case sf::Event::Closed:
					running = false;
					break;
				case sf::Event::Resized:
					res = {static_cast<float>(e.size.width), static_cast<float>(e.size.height)};
					mset_shader.setUniform("res", res);
					break;
				case sf::Event::KeyPressed:
					switch (e.key.code) {
						case sf::Keyboard::Up:
							max_iters += 10;
							mset_shader.setUniform("max_iters", max_iters);
							break;
						case sf::Keyboard::Down:
							max_iters -= 10;
							mset_shader.setUniform("max_iters", max_iters);
							break;
						case sf::Keyboard::Num1:
							set_palette(mset_shader, palette::ice);
							break;
						case sf::Keyboard::Num2:
							set_palette(mset_shader, palette::fire);
							break;
						case sf::Keyboard::Num3:
							set_palette(mset_shader, palette::forest);
							break;
					}
				case sf::Event::MouseButtonPressed:
					if (e.mouseButton.button == sf::Mouse::Button::Left) {
						drag = true;
						last_mouse_pos = {static_cast<float>(e.mouseButton.x), static_cast<float>(e.mouseButton.y)};
					}
					break;
				case sf::Event::MouseButtonReleased:
					drag = false;
					break;
				case sf::Event::MouseMoved:
					if (drag) {
						sf::Vector2f mouse_pos{static_cast<float>(e.mouseMove.x), static_cast<float>(e.mouseMove.y)};
						pos.x += (last_mouse_pos.x - mouse_pos.x) * scale / res.y;
						pos.y += (last_mouse_pos.y - mouse_pos.y) * scale / res.y;
						last_mouse_pos = mouse_pos;
						mset_shader.setUniform("pos", pos);
						break;
					}
				case sf::Event::MouseWheelScrolled:
					if (e.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel) {
						scale = scale * (1.0f - 0.1f * e.mouseWheelScroll.delta);
						mset_shader.setUniform("scale", scale);

						max_iters = get_max_iters(scale);
						mset_shader.setUniform("max_iters", max_iters);
					}
					break;
			}
		}

		// Draw.
		window.clear();
		window.draw(shape, mset_states);
		window.display();

		// Timing.
		auto now = std::chrono::steady_clock::now();
		auto elapsed = now - last_update;
		last_update = now;
		if (std::chrono::duration_cast<std::chrono::milliseconds>(elapsed) < frame_duration) {
			std::this_thread::sleep_for(frame_duration - elapsed);
		}
	}

	return 0;
}
