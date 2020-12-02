#include "plugin.hpp"


struct RawGlitters : Module {
	enum ParamIds {
		ENABLED_PARAM,
		DEREZ_PARAM,
		NUM_PARAMS
	};
	enum InputIds {
		INPUT_1_INPUT,
		INPUT_2_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		OUTPUT_1_OUTPUT,
		OUTPUT_2_OUTPUT,
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

	RawGlitters() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(ENABLED_PARAM, 0.f, 1.f, 0.f, "");
		configParam(DEREZ_PARAM, 0.f, 1.f, 0.f, "");
	}

	void process(const ProcessArgs& args) override {
	    bool highRez = params[HIGHREZ_PARAM] > 0.f;

		float enabled = params[ENABLED_PARAM].getValue();
		float fDeRez = params[DEREZ_PARAM].getValue();

		float inputSampleL = inputs[INPUT_1_INPUT].getVoltage();
		float inputSampleR = inputs[INPUT_2_INPUT].getVoltage();

		if (enabled == 1) {
			float scaleFactor = 32768.0;
			if (highRez) scaleFactor = 8388608.0;
	    	if (fDeRez > 0.0) scaleFactor *= pow(1.0-fDeRez,6);
	    	if (scaleFactor < 0.0001) scaleFactor = 0.0001;
	    	float outScale = scaleFactor;
	    	if (outScale < 8.0) outScale = 8.0;


	    	if (fabs(inputSampleL)<1.18e-37) inputSampleL = fpd * 1.18e-37;
		    fpd ^= fpd << 13; 
		    fpd ^= fpd >> 17; 
		    fpd ^= fpd << 5;
		    
		    if (fabs(inputSampleR)<1.18e-37) inputSampleR = fpd * 1.18e-37;
		    fpd ^= fpd << 13; 
		    fpd ^= fpd >> 17; 
		    fpd ^= fpd << 5;

		    inputSampleL *= scaleFactor;
		    inputSampleR *= scaleFactor;

		    float outputSampleL;
		    float outputSampleR;

		    inputSampleL += 0.381966011250105;
		    inputSampleR += 0.381966011250105;

		    if ((lastSampleL+lastSampleL) <= (inputSampleL+lastSample2L)) outputSampleL = floor(lastSampleL);
		    else outputSampleL = floor(lastSampleL+1.0);

		    if ((lastSampleR+lastSampleR) <= (inputSampleR+lastSample2R)) outputSampleR = floor(lastSampleR);
		    else outputSampleR = floor(lastSampleR+1.0);


		    lastSample2L = lastSampleL;
			lastSampleL = inputSampleL;

		    lastSample2R = lastSampleR;
		    lastSampleR = inputSampleR;


		    outputs[OUTPUT_1_OUTPUT].setVoltage(outputSampleL / outScale);
			outputs[OUTPUT_2_OUTPUT].setVoltage(outputSampleR / outScale);
		} else {
			outputs[OUTPUT_1_OUTPUT].setVoltage(inputSampleL);
			outputs[OUTPUT_2_OUTPUT].setVoltage(inputSampleR);
		}
	}
};


struct RawGlittersWidget : ModuleWidget {
	RawGlittersWidget(RawGlitters* module) {
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/rawGlitters.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(180.0, 457.35)), module, RawGlitters::ENABLED_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(180.0, 873.075)), module, RawGlitters::DEREZ_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(116.685, 1142.18)), module, RawGlitters::INPUT_1_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(243.768, 1142.18)), module, RawGlitters::INPUT_2_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(116.685, 1308.85)), module, RawGlitters::OUTPUT_1_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(243.768, 1308.85)), module, RawGlitters::OUTPUT_2_OUTPUT));
	}
};


Model* modelRawGlitters = createModel<RawGlitters, RawGlittersWidget>("rawGlitters");