#include "plugin.hpp"


struct RawGlitters : Module {

    void glitter(float scaleFactor, float inputSample, std::string lastSample);

	enum ParamIds {
		ENABLED_PARAM,
		RES_PARAM,
		DEREZ_PARAM,
		NUM_PARAMS
	};
	enum InputIds {
		INPUT_1_INPUT,
		INPUT_2_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		OUTPUT_2_OUTPUT,
		OUTPUT_1_OUTPUT,
		NUM_OUTPUTS
	};
	enum LightIds {
		NUM_LIGHTS
	};
	uint32_t fpd = 17;
    float lastSampleL = 0.0;
    float lastSample2L = 0.0;
    float lastSampleR = 0.0;
    float lastSample2R = 0.0;

    uint32_t fpd = 17;
    float lastSampleL = 0.0;
    float lastSample2L = 0.0;
    float lastSampleR = 0.0;
    float lastSample2R = 0.0;

	RawGlitters() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(ENABLED_PARAM, 0.f, 1.f, 0.f, "");
		configParam(RES_PARAM, 0.f, 1.f, 0.f, "");
		configParam(DEREZ_PARAM, 0.f, 1.f, 0.f, "");
	}

	void process(const ProcessArgs& args) override {
<<<<<<< HEAD
	    float highRez = params[RES_PARAM];
=======
	    bool highRez = params[RES_PARAM].getValue() > 0.f;
>>>>>>> ce858ddb2cbc6fcde668d46235afee1af819fb4c

		float enabled = params[ENABLED_PARAM].getValue();
		float fDeRez = params[DEREZ_PARAM].getValue();

		float inputSampleL = inputs[INPUT_1_INPUT].getVoltage();
		float inputSampleR = inputs[INPUT_2_INPUT].getVoltage();

		if (enabled == 1) {
			float scaleFactor = 32768.0;
			if (highRez == 0) scaleFactor = 8388608.0;
	    	if (fDeRez > 0.0) scaleFactor *= pow(1.0-fDeRez,6);
	    	if (scaleFactor < 0.0001) scaleFactor = 0.0001;
	    	float outScale = scaleFactor;
	    	if (outScale < 8.0) outScale = 8.0;

	    	glitter(scaleFactor, inputSampleL, "left");
            glitter(scaleFactor, inputSampleR, "right");

		} else {
			outputs[OUTPUT_1_OUTPUT].setVoltage(inputSampleL);
			outputs[OUTPUT_2_OUTPUT].setVoltage(inputSampleR);
		}
	}


};

void RawGlitters::glitter(float scaleFactor, float inputSample, std::string direction) {
    float outScale = scaleFactor;


    float lastSample = direction == "left" ? lastSampleL : lastSampleR;
    float lastSample2 = direction == "left" ? lastSample2L : lastSample2R;

    if (fabs(inputSample)<1.18e-37) inputSample = fpd * 1.18e-37;
    fpd ^= fpd << 13;
    fpd ^= fpd >> 17;
    fpd ^= fpd << 5;

    inputSample *= scaleFactor;

    float outputSample;

    inputSample += 0.381966011250105;

    if (2*lastSample <= (inputSample+lastSample2)) outputSample = floor(lastSample);
    else outputSample = floor(lastSample + 1.0);


    if (direction == "left") {
        lastSample2L = lastSample;
        outputs[OUTPUT_1_OUTPUT].setVoltage(outputSample / outScale);
    } else {
        lastSample2R = lastSample;
        outputs[OUTPUT_2_OUTPUT].setVoltage(outputSample / outScale);
    }
}

struct RawGlittersWidget : ModuleWidget {
	RawGlittersWidget(RawGlitters* module) {
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/rawGlitters.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<CKSS>(mm2px(Vec(9.958, 40.362)), module, RawGlitters::ENABLED_PARAM));
		addParam(createParamCentered<CKSS>(mm2px(Vec(20.542, 40.411)), module, RawGlitters::RES_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(15.25, 72.482)), module, RawGlitters::DEREZ_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(8.819, 96.346)), module, RawGlitters::INPUT_1_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(21.64, 96.346)), module, RawGlitters::INPUT_2_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(21.682, 113.214)), module, RawGlitters::OUTPUT_2_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(8.846, 113.333)), module, RawGlitters::OUTPUT_1_OUTPUT));
	}
};


Model* modelRawGlitters = createModel<RawGlitters, RawGlittersWidget>("rawGlitters");