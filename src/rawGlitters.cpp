#include "plugin.hpp"


struct RawGlitters : Module {
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

	RawGlitters() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(ENABLED_PARAM, 0.f, 1.f, 0.f, "");
		configParam(RES_PARAM, 0.f, 1.f, 0.f, "");
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