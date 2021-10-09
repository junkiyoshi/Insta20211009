#include "ofApp.h"	

//--------------------------------------------------------------
Actor::Actor(vector<glm::vec3>& location_list, vector<vector<int>>& next_index_list, vector<int>& destination_list) {

	this->select_index = ofRandom(location_list.size());
	while (true) {

		auto itr = find(destination_list.begin(), destination_list.end(), this->select_index);
		if (itr == destination_list.end()) {

			destination_list.push_back(this->select_index);
			break;
		}

		this->select_index = (this->select_index + 1) % location_list.size();
	}

	this->next_index = this->select_index;

	this->noise_param = ofRandom(1000);
	this->noise_step = ofRandom(1000);
}

//--------------------------------------------------------------
void Actor::update(const int& frame_span, vector<glm::vec3>& location_list, vector<vector<int>>& next_index_list, vector<int>& destination_list) {

	if (ofGetFrameNum() % frame_span == 0) {

		auto tmp_index = this->select_index;
		this->select_index = this->next_index;
		int retry = next_index_list[this->select_index].size();
		this->next_index = next_index_list[this->select_index][(int)ofRandom(next_index_list[this->select_index].size())];
		while (--retry > 0) {

			auto destination_itr = find(destination_list.begin(), destination_list.end(), this->next_index);
			if (destination_itr == destination_list.end()) {

				if (tmp_index != this->next_index) {

					destination_list.push_back(this->next_index);
					break;
				}
			}

			this->next_index = next_index_list[this->select_index][(this->next_index + 1) % next_index_list[this->select_index].size()];
		}
		if (retry <= 0) {

			destination_list.push_back(this->select_index);
			this->next_index = this->select_index;
		}
	}

	auto param = ofGetFrameNum() % frame_span;
	auto distance = location_list[this->next_index] - location_list[this->select_index];
	this->location = location_list[this->select_index] + distance / frame_span * param;

	if (this->next_index != this->select_index) {

		this->noise_step += 0.05;
	}
}

//--------------------------------------------------------------
glm::vec3 Actor::getLocation() {

	return this->location;
}

//--------------------------------------------------------------
float Actor::getWordIndexNoiseValue() {

	return ofNoise(this->noise_param, this->noise_step);
}

//--------------------------------------------------------------
void ofApp::setup() {

	ofSetFrameRate(60);
	ofSetWindowTitle("openFrameworks");

	ofBackground(239);
	ofSetColor(39);

	auto span = 30;
	for (int x = -240; x <= 240; x += span) {

		for (int y = -240; y <= 240; y += span) {

			this->location_list.push_back(glm::vec3(x, y, 0));
		}
	}

	for (auto& location : this->location_list) {

		vector<int> next_index = vector<int>();
		int index = -1;
		for (auto& other : this->location_list) {

			index++;
			if (location == other) { continue; }

			float distance = glm::distance(location, other);
			if (distance <= span) {

				next_index.push_back(index);
			}
		}

		this->next_index_list.push_back(next_index);
	}

	for (int i = 0; i < 255; i++) {

		this->actor_list.push_back(make_unique<Actor>(this->location_list, this->next_index_list, this->destination_list));
	}

	this->font_size = 23;
	ofTrueTypeFontSettings font_settings("fonts/msgothic.ttc", this->font_size);
	font_settings.antialiased = true;
	font_settings.addRanges(ofAlphabet::Japanese);
	this->font.load(font_settings);

	this->words = {

		u8"あ", u8"い", u8"う", u8"え", u8"お",
		u8"か", u8"き", u8"く", u8"け", u8"こ",
		u8"さ", u8"し", u8"す", u8"せ", u8"そ",
		u8"た", u8"ち", u8"つ", u8"て", u8"と",
		u8"な", u8"に", u8"ぬ", u8"ね", u8"の",
		u8"は", u8"ひ", u8"ふ", u8"へ", u8"ほ",
		u8"ま", u8"み", u8"む", u8"め", u8"も",
		u8"や", u8"ゆ", u8"よ",
		u8"ら", u8"り", u8"る", u8"れ", u8"ろ",
		u8"わ", u8"を", u8"ん",
	};
}

//--------------------------------------------------------------
void ofApp::update() {

	int frame_span = 30;
	int prev_index_size = 0;

	if (ofGetFrameNum() % frame_span == 0) {

		prev_index_size = this->destination_list.size();
	}

	for (auto& actor : this->actor_list) {

		actor->update(frame_span, this->location_list, this->next_index_list, this->destination_list);
	}

	if (prev_index_size != 0) {

		this->destination_list.erase(this->destination_list.begin(), this->destination_list.begin() + prev_index_size);
	}
}

//--------------------------------------------------------------
void ofApp::draw() {

	this->cam.begin();

	for (auto& actor : this->actor_list) {

		this->font.drawString(this->words[ofMap(actor->getWordIndexNoiseValue(), 0, 1, 0, this->words.size())], actor->getLocation().x - 15, actor->getLocation().y - 15);
	}

	this->cam.end();
}


//--------------------------------------------------------------
int main() {

	ofSetupOpenGL(720, 720, OF_WINDOW);
	ofRunApp(new ofApp());
}