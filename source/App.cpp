#include "App.h"

#include <math.h> 

G3D_START_AT_MAIN();

int main(int argc, const char* argv[]) {
    G3D::G3DSpecification spec;
    spec.audio=true;
    
    initGLG3D(spec);

    GApp::Settings settings(argc, argv);
    //settings.window.framed = false; 
    settings.window.caption     = "DOTD: Defense of the DiscoBall!";
    settings.window.width       = 1280; 
    settings.window.height      = 720;
    try {
        settings.window.defaultIconFilename = System::findDataFile("icon/rocket/icon.png");
    } catch (...) {
        debugPrintf("Could not find icon\n");
        // Ignore exception if we can't find the icon
    }

    return App(settings).run();
}


App::App(const GApp::Settings& settings) : GApp(settings) {
}


void App::onInit() {
    GApp::onInit();
    // Called before the application loop begins.  Load data here and
    // not in the constructor so that common exceptions will be
    // automatically caught.
    showRenderingStats    = false;

    // For higher-quality screenshots:
    // developerWindow->videoRecordDialog->setScreenShotFormat("PNG");
    // developerWindow->videoRecordDialog->setCaptureGui(false);

    setFrameDuration(1.0f / 60);

    // Buffer 
    m_gbufferSpecification.encoding[GBuffer::Field::CS_FACE_NORMAL].format = NULL;

    m_gbufferSpecification.encoding[GBuffer::Field::CS_NORMAL] = Texture::Encoding(ImageFormat::RGB10A2(), FrameName::CAMERA, 2.0f, -1.0f);

    m_gbufferSpecification.encoding[GBuffer::Field::SS_EXPRESSIVE_MOTION]    = 
        Texture::Encoding(GLCaps::supportsTexture(ImageFormat::RG8()) ? ImageFormat::RG8() : ImageFormat::RGBA8(),
        FrameName::SCREEN, 128.0f, -64.0f);

    m_gbufferSpecification.encoding[GBuffer::Field::EMISSIVE]           =  
        GLCaps::supportsTexture(ImageFormat::RGB5()) ? 
            Texture::Encoding(ImageFormat::RGB5(), FrameName::NONE, 1.0f, 0.0f) : 
            Texture::Encoding(ImageFormat::R11G11B10F());

    m_gbufferSpecification.encoding[GBuffer::Field::LAMBERTIAN]         = ImageFormat::RGB8();
    m_gbufferSpecification.encoding[GBuffer::Field::GLOSSY]             = ImageFormat::RGBA8();
    m_gbufferSpecification.encoding[GBuffer::Field::DEPTH_AND_STENCIL]  = ImageFormat::DEPTH32F();
    m_gbufferSpecification.depthEncoding = DepthEncoding::HYPERBOLIC;

    // Update the actual m_gbuffer before makeGUI creates the buffer visualizer
    m_gbuffer->setSpecification(m_gbufferSpecification);
    m_gbuffer->resize(renderDevice->width() + m_settings.depthGuardBandThickness.x * 2, renderDevice->height() + m_settings.depthGuardBandThickness.y * 2);

    renderDevice->setSwapBuffersAutomatically(false);

    m_tmpbuffer = Framebuffer::create(Texture::createEmpty("foo", m_framebuffer->width(), m_framebuffer->height()));

    //---------------

    m_lastLightingChangeTime = 0.0;
    m_toonShader = Shader::fromFiles("toon.pix");
    m_scene = DemoScene::create();
    //m_scene->registerEntitySubclass("PlayerEntity", &PlayerEntity::create);
    setScene(m_scene);

    makeGUI();

    developerWindow->cameraControlWindow->moveTo(Point2(developerWindow->cameraControlWindow->rect().x0(), 0));

    loadScene(System::findDataFile("space.Scene.Any"));
    // Enforce correct simulation order
    //m_scene->setOrder("player", "camera");
    m_map = shared_ptr<Map>(new Map::Map(m_scene));
    vControl = VisualControl::create(m_scene, updateRate, m_map); 



    //CHANGES DEBUGGING 
    //This should from Kai 

    //for(int i = 0; i<10; i++){
    //shared_ptr<VisibleEntity> creature = m_scene -> spawnVisibleEntity("creature","creatureModel",CFrame::fromXYZYPRDegrees(1,1,1,0,0,0) ); 

    //shared_ptr<Change> change(new Change( Vector3(1,0,1), Vector3(1,1,1), 100, 100, Creature::type, creature) ); 

    //allChanges.push_back( change ); 
    //}

    allChanges = m_map->getChange();

    levelUp = G3D::Sound::create(System::findDataFile("levelup.wav",false));
    placeCube = G3D::Sound::create(System::findDataFile("placecube.wav",false));

    //m_map->add( m_scene -> spawnVisibleEntity("creature","creatureModel",CFrame::fromXYZYPRDegrees(1,1,1,0,0,0)), Creeper::type, 1,1,1 ); 
                

    //CHANGES END 
    /* Crashes the program 
    for(int x = 0; x<15; ++x){
        for(int z = 0; z<15; ++z){
            m_map->add( m_scene -> spawnVisibleEntity( "hi", "cubeModel", CFrame::fromXYZYPRDegrees(x,0,z,0,0,0)), Cube::type, x, 0, z); 
        }
    }
    */

}




void App::makeGUI() {
    // Initialize the developer HUD
    createDeveloperHUD();

    debugWindow->setVisible(false);
    developerWindow->videoRecordDialog->setEnabled(true);
    developerWindow->sceneEditorWindow->setVisible(false);
    developerWindow->cameraControlWindow->setVisible(false);

    debugWindow->pack();
    debugWindow->setRect(Rect2D::xywh(0, 0, (float)window()->width(), debugWindow->rect().height()));

    shared_ptr<GFont> arialFont = GFont::fromFile(System::findDataFile("icon.fnt"));
    shared_ptr<GuiTheme> theme = GuiTheme::fromFile(System::findDataFile("osx-10.7.gtm"), arialFont);
    
    shared_ptr<GuiWindow> mainWindow = GuiWindow::create("Main",theme, Rect2D::xywh(0,150,250,150),GuiTheme::TOOL_WINDOW_STYLE, GuiWindow::HIDE_ON_CLOSE);

    GuiPane* MainPane = mainWindow->pane();
    addWidget(mainWindow);
    
    GuiPane* statsPane = MainPane->addPane("Stats", GuiTheme::ORNATE_PANE_STYLE);
    statsPane->addNumberBox("Gold: ", &m_gold)->setEnabled(false);
    statsPane->addNumberBox("HP: ", &m_hp)->setEnabled(false);
    statsPane->addNumberBox("Level: ", &levelProgression)->setEnabled(false);
    statsPane->addButton("START GAME", this, &App::onHelp);

    statsPane->addLabel("WASD QE : move and rotate camera",GFont::XALIGN_LEFT,GFont::YALIGN_TOP);
    statsPane->addLabel("LEFT CLICK : build cube ",GFont::XALIGN_LEFT,GFont::YALIGN_TOP);
    statsPane->addLabel("TAB + LEFT CLICK : build tower ",GFont::XALIGN_LEFT,GFont::YALIGN_TOP);
    statsPane->addLabel("SHIFT + LEFT CLICK : delete ",GFont::XALIGN_LEFT,GFont::YALIGN_TOP);




    shared_ptr<GuiWindow> helpWindow = GuiWindow::create("Help",theme, Rect2D::xywh(0,10,250,150),GuiTheme::TOOL_WINDOW_STYLE, GuiWindow::HIDE_ON_CLOSE);

    GuiPane* HelpPane = helpWindow->pane(); 
    addWidget(helpWindow); 

    GuiPane* instructionPane = HelpPane->addPane("Instructions", GuiTheme::ORNATE_PANE_STYLE);
    instructionPane->addLabel("Defend the DiscoBall! (that big sphere in the center)",GFont::XALIGN_LEFT,GFont::YALIGN_TOP);
    instructionPane->addLabel("Enemies close to it do damage",GFont::XALIGN_LEFT,GFont::YALIGN_TOP);
    instructionPane->addLabel("Kill enemies to get GOLD to build cubes and towers to stop them",GFont::XALIGN_LEFT,GFont::YALIGN_TOP);

//  The enemy will rush towards it and decrease its health.  Get GOLD from killing enemies.  Use gold to build cubes and towers.\n Build Cube (Left Click) costs 20 gold \n Build Tower (Tab + Left Click on Cube) costs 500 gold \n Delete Cube (Shift + Left Click on Cube) no refunds ", GFont::XALIGN_LEFT, GFont::YALIGN_TOP);//, GFont::YALIGN_CENTER); 

    instructionPane->pack(); 

    


    //Instruction Pane 
    //shared_ptr<GuiWindow> instructionPane = GuiWindow::create("Instructions",theme, Rect2D::xywh(0,0,250,150),GuiTheme::TOOL_WINDOW_STYLE, GuiWindow::HIDE_ON_CLOSE);

    //instructionPane->add

    statsPane->pack();
}

//rotate p around cx and cy 
Vector3 App::rotatePoint(float cx,float cz,float angle,Vector3 p)
{
    float s = sin(angle);
    float c = cos(angle);
    // translate point back to origin:
    p.x -= cx;
    p.z -= cz;

    // rotate point
    float xnew = p.x * c - p.z * s;
    float znew = p.x * s + p.z * c;

    // translate point back:
    p.x = xnew + cx;
    p.z = znew + cz;
    return p;
}



void App::onGraphics3D(RenderDevice* rd, Array<shared_ptr<Surface> >& allSurfaces) {
    // This implementation is equivalent to the default GApp's. It is repeated here to make it
    // easy to modify rendering. If you don't require custom rendering, just delete this
    // method from your application and rely on the base class.
    if (! scene()) {
        return;
    }
    m_gbuffer->setSpecification(m_gbufferSpecification);
    m_gbuffer->resize(m_framebuffer->width(), m_framebuffer->height());

    // Share the depth buffer with the forward-rendering pipeline
    m_framebuffer->set(Framebuffer::DEPTH, m_gbuffer->texture(GBuffer::Field::DEPTH_AND_STENCIL));
    m_depthPeelFramebuffer->resize(m_framebuffer->width(), m_framebuffer->height());

    Surface::AlphaMode coverageMode = Surface::ALPHA_BLEND;

    // Bind the main framebuffer
    rd->pushState(m_framebuffer); {
        rd->clear();
        rd->setProjectionAndCameraMatrix(activeCamera()->projection(), activeCamera()->frame());

        m_gbuffer->prepare(rd, activeCamera(), 0, -(float)previousSimTimeStep(), m_settings.depthGuardBandThickness, m_settings.colorGuardBandThickness);
        
        // Cull and sort
        Array<shared_ptr<Surface> > sortedVisibleSurfaces;
        Surface::cull(activeCamera()->frame(), activeCamera()->projection(), rd->viewport(), allSurfaces, sortedVisibleSurfaces);
        Surface::sortBackToFront(sortedVisibleSurfaces, activeCamera()->frame().lookVector());
        
        const bool renderTransmissiveSurfaces = false;

        // Intentionally copy the lighting environment for mutation
        LightingEnvironment environment = scene()->lightingEnvironment();
        environment.ambientOcclusion = m_ambientOcclusion;
       
        // Render z-prepass and G-buffer.
        Surface::renderIntoGBuffer(rd, sortedVisibleSurfaces, m_gbuffer, activeCamera()->previousFrame(), activeCamera()->expressivePreviousFrame(), renderTransmissiveSurfaces, coverageMode);

        // This could be the OR of several flags; the starter begins with only one motivating algorithm for depth peel
        const bool needDepthPeel = environment.ambientOcclusionSettings.useDepthPeelBuffer;
        if (needDepthPeel) {
            rd->pushState(m_depthPeelFramebuffer); {
                rd->clear();
                rd->setProjectionAndCameraMatrix(activeCamera()->projection(), activeCamera()->frame());
                Surface::renderDepthOnly(rd, sortedVisibleSurfaces, CullFace::BACK, renderTransmissiveSurfaces, coverageMode, m_framebuffer->texture(Framebuffer::DEPTH), environment.ambientOcclusionSettings.depthPeelSeparationHint);
            } rd->popState();
        }

        if (! m_settings.colorGuardBandThickness.isZero()) {
            rd->setGuardBandClip2D(m_settings.colorGuardBandThickness);
        }        

        // Compute AO
        m_ambientOcclusion->update(rd, environment.ambientOcclusionSettings, activeCamera(), m_framebuffer->texture(Framebuffer::DEPTH), m_depthPeelFramebuffer->texture(Framebuffer::DEPTH), m_gbuffer->texture(GBuffer::Field::CS_NORMAL), m_gbuffer->texture(GBuffer::Field::SS_POSITION_CHANGE), m_settings.depthGuardBandThickness - m_settings.colorGuardBandThickness);

        const RealTime lightingChangeTime = max(scene()->lastEditingTime(), max(scene()->lastLightChangeTime(), scene()->lastVisibleChangeTime()));
        bool updateShadowMaps = false;
        if (lightingChangeTime > m_lastLightingChangeTime) {
            m_lastLightingChangeTime = lightingChangeTime;
            updateShadowMaps = true;
        }
        // No need to write depth, since it was covered by the gbuffer pass
        rd->setDepthWrite(false);
        // Compute shadow maps and forward-render visible surfaces
        Surface::render(rd, activeCamera()->frame(), activeCamera()->projection(), sortedVisibleSurfaces, allSurfaces, environment, coverageMode, updateShadowMaps, m_settings.depthGuardBandThickness - m_settings.colorGuardBandThickness, sceneVisualizationSettings());      
                
        // Call to make the App show the output of debugDraw(...)
        drawDebugShapes();
        const shared_ptr<Entity>& selectedEntity = (notNull(developerWindow) && notNull(developerWindow->sceneEditorWindow)) ? developerWindow->sceneEditorWindow->selectedEntity() : shared_ptr<Entity>();
        scene()->visualize(rd, selectedEntity, sceneVisualizationSettings());


        // Post-process special effects
        m_depthOfField->apply(rd, m_framebuffer->texture(0), m_framebuffer->texture(Framebuffer::DEPTH), activeCamera(), m_settings.depthGuardBandThickness - m_settings.colorGuardBandThickness);
        
        //m_motionBlur->apply(rd, m_framebuffer->texture(0), m_gbuffer->texture(GBuffer::Field::SS_EXPRESSIVE_MOTION), 
        //                    m_framebuffer->texture(Framebuffer::DEPTH), activeCamera(), 
        //                    m_settings.depthGuardBandThickness - m_settings.colorGuardBandThickness);

    } rd->popState();
    shared_ptr<Texture> text(m_framebuffer->texture(0));
    //text = Texture::one();
    rd->push2D(m_tmpbuffer); {
        Args args;
        args.setRect(rd->viewport());

        args.setUniform("Texture", text, Sampler::video());
        args.setUniform("HP", m_hp);
        args.setUniform("flashCount", flashCount);
        m_gbuffer->setShaderArgsRead(args, "gbuffer_");

        LAUNCH_SHADER("toon2.pix", args);


        drawDebugShapes();

    } rd->pop2D();

    swapBuffers();

    rd->clear();

    // Perform gamma correction, bloom, and SSAA, and write to the native window frame buffer
    m_film->exposeAndRender(rd, activeCamera()->filmSettings(), m_tmpbuffer->texture(0));
}

void App::onUserInput(UserInput* ui) {
    GApp::onUserInput(ui);

    Vector3 translation = activeCamera()->frame().translation; 

    //adjust camera zoom 
    if( ui->keyDown( GKey('w')) && curDist > 0){
        curDist -= .3f; 
        //frame.translation += frame.lookVector(); 
    }
    else if( ui->keyDown( GKey('s'))){
        curDist += .3f; 
        ///frame.translation -= frame.lookVector(); 
    }

    //rotate camera around point 
    if( ui->keyDown( GKey('a'))){
        curAngle += .1f; 

    }
    else if( ui->keyDown( GKey('d'))){
        curAngle -= .1f; 
        //translation = rotatePoint(7,7,curAngle,Vector3(curDist,0,0)); 
    }
    translation = rotatePoint(7,7,curAngle,Vector3(curDist,0,0));

    //adjust camera height 
    if( ui->keyDown( GKey('z'))){
        curHeight -= .3f; 
    }
    else if( ui->keyDown( GKey('c'))){
        curHeight += .3f; 
    }
    if (activeCamera()->name() == "camera"){
        activeCamera()->setFrame( CoordinateFrame::fromXYZYPRDegrees( translation.x, curHeight, translation.z, curAngle, -30, 0)); 


        CoordinateFrame frame = CoordinateFrame(activeCamera()->frame()); 
        frame.lookAt(Vector3(7,2,7));


        activeCamera()->setFrame(frame); 
    }

    

    /*
    if( ui->keyDown( GKey('w') )){
        Vector3 lookVec = frame.lookVector();
        lookVec.y = 0; 

        translation += lookVec; 
     //translation.z = translation.z - .3f; 
    }
    if( ui->keyDown( GKey('a') )){
        translation -= frame.rightVector();
        //translation.x = translation.x - .3f; 
    }
    if( ui->keyDown( GKey('s') )){
        Vector3 lookVec = frame.lookVector();
        lookVec.y = 0; 

        translation -= lookVec; 
     //translation.z = translation.z + .3f; 
    }
    if( ui->keyDown( GKey('d') )){
        translation += frame.rightVector();
        //translation.x = translation.x + .3f; 
    }
    if( ui->keyDown( GKey('q') )){
        cameraYaw += 2; 
    }
    else if( ui->keyDown( GKey('e'))){
        cameraYaw -= 2; 
    }

    activeCamera()->setFrame( CoordinateFrame::fromXYZYPRDegrees( translation.x, translation.y, translation.z, cameraYaw, -30, 0)); 
    */
    



    if (pause){
        if (ui -> keyPressed( G3D::GKey::LEFT_MOUSE)){
            menuPage += 1;
        }
        if (menuPage == 9){
            pause = false;
        }
    } else{

        //Upgrade a box 
        if( ui -> keyPressed( G3D::GKey::LEFT_MOUSE) && ui->keyDown( G3D::GKey::TAB) && m_map->canAdd( Cube::type, BasicTower::creatureType )){
            Vector2 mousePos = ui->mouseXY(); 
            int xPos = (int)floor(mousePos.x); 
            int yPos = (int)floor(mousePos.y); 
            
            shared_ptr<MapElement> selectedElement = m_map->getSelectedEntity(activeCamera()->worldRay( xPos, yPos, Rect2D(Vector2( 1280, 720 ))), Cube::type); 


            //If element selected and it is not falling 
            if( !isNull(selectedElement) && selectedElement->newPos == selectedElement->oldPos && selectedElement->newPos.y>1){
             
                //Remove Old Cube 
                m_scene->removeVisibleEntity( selectedElement->entity ); 
                m_map->remove( selectedElement->entity ); 

                //Initialize new tower 
                IntVec3 oldPos = selectedElement->oldPos; 
                m_map->add( m_scene -> spawnVisibleEntity( "hi", "towerModel", CFrame::fromXYZYPRDegrees(oldPos.x,oldPos.y,oldPos.z,0,0,0)), Cube::type, BasicTower::type, oldPos.x, oldPos.y, oldPos.z, 1.0); 
            }
        }

        //Remove a Box 
        else if( ui -> keyPressed( G3D::GKey::LEFT_MOUSE) && ui->keyDown( G3D::GKey::LSHIFT)){
            Vector2 mousePos = ui->mouseXY(); 
            int xPos = (int)floor(mousePos.x); 
            int yPos = (int)floor(mousePos.y); 

            shared_ptr<MapElement> removeCube = m_map->getSelectedEntity(activeCamera()->worldRay( xPos, yPos, Rect2D(Vector2( 1280, 720 ))), Cube::type); 

            if(!isNull(removeCube)){
                shared_ptr<VisibleEntity> removeCubeVE = removeCube->entity; 

                //if( !isNull(removeCubeVE) ){
                    m_scene->removeVisibleEntity( removeCubeVE ); 
                    m_map->remove( removeCubeVE ); 
                    //}

            }


            //vControl->removeCubeAt( activeCamera()->worldRay( xPos, yPos, Rect2D(Vector2( 1280, 720 )))); 
        }

        //Add a box 
        else if( ui -> keyPressed( G3D::GKey::LEFT_MOUSE) && ! ui->keyDown(G3D::GKey::TAB) && m_map->canAdd( Cube::type, Cube::creatureType) ){
            Vector2 mousePos = ui->mouseXY(); 
            int xPos = (int)floor(mousePos.x); 
            int yPos = (int)floor(mousePos.y); 
            
            IntVec3 buildPos = m_map->getSelectedBuildPos(activeCamera()->worldRay( xPos, yPos, Rect2D(Vector2( 1280, 720 )))); 
            //debugPrintf( "%i, %i, %i", buildPos.x, buildPos.y, buildPos.z); 


            if(m_map->canAddBox( buildPos.x, buildPos.y, buildPos.z )){
                placeCube->play(1.0);
                m_map->add( m_scene -> spawnVisibleEntity( "hi", "cubeModel", CFrame::fromXYZYPRDegrees(buildPos.x,buildPos.y,buildPos.z,0,0,0)), Cube::type, buildPos.x, buildPos.y, buildPos.z ); 
            }
            //vControl->createCubeAt( activeCamera()->worldRay( xPos, yPos, Rect2D(Vector2( 1280, 720 )))); 
        }
    }

}



void App::onSimulation(RealTime rdt, SimTime sdt, SimTime idt) {

    GApp::onSimulation(rdt, sdt, idt);
    if (! pause){
        spawnCeature();

        m_hp = m_map->discoBallHP;
        if (m_hp != oldHP){
            if (safeCount > 60){
                flashCount = 11;
            }
            safeCount = 0;
        } else{
            safeCount += 1;
        }
        flashCount -= 1;
        oldHP = m_hp;
        if (m_hp <= 0){
            drawMessage("Game Over");
        }


        //Deprecated, updateRate always 1 now
        if((curTime + 1) == updateRate){
            curTime = 0; 

            allChanges = m_map->getChange();
            vControl->update( allChanges ); 
        }
        else{ 
            vControl->update( allChanges ); 
            curTime ++; 
        }

        //animation step for VisualControl 
        vControl->step(); 

      //Animation Objects 
        /*
        for(int i = 0; i<(int)allAnimations.size(); i++){
            if( allAnimations[i]->step() < 0 ){

                m_scene->removeVisibleEntity( allAnimations[i]->ve ); 
                allAnimations.erase( allAnimations.begin() + i); 
            }
        }
        */


        
    } else{
        /*
        if (menuPage == 0){
            drawMessage("Welcome to Disco Ball. Click to continue");
        } else if (menuPage == 1){
            drawMessage("Your goal is to defend the divine Disco Ball");
        } else if (menuPage == 2){
            drawMessage("Left click to add cubes that form your wall of defense (20 Gold)");
        } else if (menuPage == 3){
            drawMessage("Tab+left click to upgrade your cubes into towers that attack (500 Gold)");
        } else if (menuPage == 4){
            drawMessage("Shift+left click to remove cubes and towers");
        } else if (menuPage == 5){
            drawMessage("Towers get destroyed after falling twice");
        } else if (menuPage == 6){
            drawMessage("Kill enemy to earn gold, manage your economy wisely");
        } else if (menuPage == 7){
            drawMessage("You lose if HP of Disco Ball goes below 0");
        } else{
            drawMessage("GOOD LUCK! (Switch to debug camera for now)");
        }
        */
        menuPage = 9; 
    }
}

    
float App::randomFloat( float start, float end ){
    return start + static_cast<float>(rand()) / (static_cast<float> (RAND_MAX/(end-start))); 
}

void App::spawnCeature(){
    int t;
    int x = 1;
    int y = 1;
    int z = 1;
    int occupancy = 0;
    if (spawnTimer >= 500){
        while (spawnTimer >= 500){
        int a = rand() % 100;

        if (a < 10){
            t = Creeper::creatureType;
            occupancy = Creeper::occupancy;
        } else if (a < 15){
            t = Crawler::creatureType;
            occupancy = Crawler::occupancy;
        } else if (a < 80){
            t = SmartCrawler::creatureType;
            occupancy = SmartCrawler::occupancy;
        } else{
            t = SuperCrawler::creatureType;
            occupancy = SuperCrawler::occupancy;
        }


        do{
            if (rand() % 2 < 1){
                x = rand() % 15;
                if (rand() % 2 < 1){
                    z = 0;
                } else{
                    z = 14;
                }
            } else{
                z = rand() % 15;
                if (rand() % 2 < 1){
                    x = 0;
                } else{
                    x = 14;
                }
            }
        } while(! m_map->canSpawnCreature(x,y,z,occupancy));
        x = (x-7)*2+7;
        z = (z-7)*2+7;
        m_map->add( m_scene -> spawnVisibleEntity("creature","creatureModel",CFrame::fromXYZYPRDegrees(x,y,z,0,0,0)), Creature::type, t, x,y,z, levelProgression*0.2+1); 

        spawnTimer -= 500; 
        }
    }
    if (spawnTimer > 0 && rand()%300 == 1){
        levelProgression += 1;
        levelUp->play(2.0);
    }
    int increment = levelProgression/4;
    increment = (increment+1)*levelProgression;
    spawnTimer += 1 + increment;
    m_gold = m_map->gold/10;
}

void App::onHelp(){
    //menuPage = 0;
    spawnTimer = 500; 
}
