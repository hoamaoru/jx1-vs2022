#ifndef KNpcAttribModifyH
#define KNpcAttribModifyH

class KNpc;

class KNpcAttribModify
{
public:
	KNpcAttribModify();
	~KNpcAttribModify();
	void	ModifyAttrib(KNpc* pNpc, void* pData);
	void	MixPoisonDamage(KMagicAttrib* pDes, KMagicAttrib* pSrc);
private:
	void	(KNpcAttribModify::*ProcessFunc[magic_normal_end])(KNpc* pNpc, void* pData);	
	void	AllResMaxP(KNpc* pNpc, void* pData);
	void	PhysicsResMaxP(KNpc* pNpc, void* pData);
	void	ColdResMaxP(KNpc* pNpc, void* pData);
	void	FireResMaxP(KNpc* pNpc, void* pData);
	void	LightingResMaxP(KNpc* pNpc, void* pData);
	void	PoisonResMaxP(KNpc* pNpc, void* pData);
	void	LifePotionV(KNpc* pNpc, void* pData);
	void	ManaPotionV(KNpc* pNpc, void* pData);
	void	PoisonEnhanceP(KNpc* pNpc, void* pData);
	void	LightingEnhanceP(KNpc* pNpc, void* pData);
	void	FireEnhanceP(KNpc* pNpc, void* pData);
	void	ColdEnhanceP(KNpc* pNpc, void* pData);
	void	ArmorDefenseV(KNpc* pNpc, void* pData);
	void	LifeMaxV(KNpc* pNpc, void* pData);
	void	LifeMaxP(KNpc* pNpc, void* pData);
	void	LifeV(KNpc* pNpc, void* pData);
	void	LifeReplenishV(KNpc* pNpc, void* pData);
	void	ManaMaxV(KNpc* pNpc, void* pData);
	void	ManaMaxP(KNpc* pNpc, void* pData);
	void	ManaV(KNpc* pNpc, void* pData);
	void	ManaReplenishV(KNpc* pNpc, void* pData);
	void	StaminaMaxV(KNpc* pNpc, void* pData);
	void	StaminaMaxP(KNpc* pNpc, void* pData);
	void	StaminaV(KNpc* pNpc, void* pData);
	void	StaminaReplenishV(KNpc* pNpc, void* pData);
	void	StrengthV(KNpc* pNpc, void* pData);
	void	DexterityV(KNpc* pNpc, void* pData);
	void	VitalityV(KNpc* pNpc, void* pData);
	void	EnergyV(KNpc* pNpc, void* pData);
	void	LuckyV(KNpc* pNpc, void* pData);
	void	PoisonresP(KNpc* pNpc, void* pData);
	void	FireresP(KNpc* pNpc, void* pData);
	void	LightingresP(KNpc* pNpc, void* pData);
	void	PhysicsresP(KNpc* pNpc, void* pData);
	void	ColdresP(KNpc* pNpc, void* pData);
	void	FreezeTimeReduceP(KNpc* pNpc, void* pData);
	void	BurnTimeReduceP(KNpc* pNpc, void* pData);
	void	PoisonTimeReduceP(KNpc* pNpc, void* pData);
	void	PoisonDamageReduceV(KNpc* pNpc, void* pData);
	void	StunTimeReduceP(KNpc* pNpc, void* pData);
	void	FastWalkRunP(KNpc* pNpc, void* pData);
	void	VisionRadiusP(KNpc* pNpc, void* pData);
	void	FastHitRecoverV(KNpc* pNpc, void* pData);
	void	AllresP(KNpc* pNpc, void* pData);
	void	AttackRatingV(KNpc* pNpc, void* pData);
	void	AttackRatingP(KNpc* pNpc, void* pData);
	void	AttackSpeedV(KNpc* pNpc, void* pData);
	void	CastSpeedV(KNpc* pNpc, void* pData);
	void	MeleeDamageReturnP(KNpc* pNpc, void* pData);
	void	MeleeDamageReturnV(KNpc* pNpc, void* pData);
	void	RangeDamageReturnV(KNpc* pNpc, void* pData);
	void	RangeDamageReturnP(KNpc* pNpc, void* pData);
	void	AddPhysicsDamageV(KNpc* pNpc, void* pData);
	void	AddFireDamageV(KNpc* pNpc, void* pData);
	void	AddColdDamageV(KNpc* pNpc, void* pData);
	void	AddLightingDamageV(KNpc* pNpc, void* pData);
	void	AddPoisonDamageV(KNpc* pNpc, void* pData);
	void	AddPhysicsDamageP(KNpc* pNpc, void* pData);
	void	SlowMissleB(KNpc* pNpc, void* pData);
	void	ChangeCampV(KNpc* pNpc, void* pData);
	void	PhysicsArmorV(KNpc* pNpc, void* pData);
	void	ColdArmorV(KNpc* pNpc, void* pData);
	void	FireArmorV(KNpc* pNpc, void* pData);
	void	PoisonArmorV(KNpc* pNpc, void* pData);
	void	LightingArmorV(KNpc* pNpc, void* pData);
	void	Damage2AddManaP(KNpc* pNpc, void* pData);
	void	StealLifeP(KNpc* pNpc, void* pData);
	void	StealStaminaP(KNpc* pNpc, void* pData);
	void	StealManaP(KNpc* pNpc, void* pData);
	void	AllSkillV(KNpc* pNpc, void* pData);
	void	MetalSkillV(KNpc* pNpc, void* pData);
	void	WoodSkillV(KNpc* pNpc, void* pData);
	void	WaterSkillV(KNpc* pNpc, void* pData);
	void	FireSkillV(KNpc* pNpc, void* pData);
	void	EarthSkillV(KNpc* pNpc, void* pData);
	void	FatallyStrikeResP(KNpc* pNpc, void* pData);
	void	DeadlyStrikeEnhanceP(KNpc* pNpc, void* pData);
	void	FattallyStrikeEnhanceP(KNpc* pNpc, void* pData);
	void	BadStatusTimeReduceV(KNpc* pNpc, void* pData);
	void	ManaShieldP(KNpc* pNpc, void* pData);
	void	FatallyStrikeEnhanceP(KNpc* pNpc, void* pData);
	void	AddColdMagicV(KNpc* pNpc, void* pData);
	void	AddFireMagicV(KNpc* pNpc, void* pData);
	void	AddLightingMagicV(KNpc* pNpc, void* pData);
	void	AddPoisonMagicV(KNpc* pNpc, void* pData);
	void	AddPhysicsMagicV(KNpc* pNpc, void* pData);

	void	DynamicMagicShieldV(KNpc* pNpc, void* pData);
	void	StaticMagicShieldP(KNpc* pNpc, void* pData);
	void	ExpEnhanceP(KNpc* pNpc, void* pData);
	void	ExpSkillsEnhanceP(KNpc* pNpc, void* pData); //TamLTM ExpSkills x2
	void	LifeReplenishP(KNpc* pNpc, void* pData);
	void	IgnoreSkillP(KNpc* pNpc, void* pData);
	void	PoisonDamageReturnP(KNpc* pNpc, void* pData);
	void	ReturnSkillP(KNpc* pNpc, void* pData);
	void	Poison2DecManaP(KNpc* pNpc, void* pData);
	void	AutoDeathSkill(KNpc* pNpc, void* pData);
	void	AutoAttackSkill(KNpc* pNpc, void* pData);
	void	Hide(KNpc* pNpc, void* pData);
	void	IgnoreNegativeStateP(KNpc* pNpc, void* pData);
	void	AutoReplySkill(KNpc* pNpc, void* pData);
	void	AutoRescueSkill(KNpc* pNpc, void* pData);
	void	ReturnResP(KNpc* pNpc, void* pData);
	void	SkillEnhanceP(KNpc* pNpc, void* pData);
	void	FiveElementsEnhanceV(KNpc* pNpc, void* pData);
	void	FiveElementsResistV(KNpc* pNpc, void* pData);	
	void	FrozenAction(KNpc* pNpc, void* pData);	
	void	WalkRunShadow(KNpc* pNpc, void* pData);	
	void	RandMove(KNpc* pNpc, void* pData);	
	void	ManaToSkillEnhanceP(KNpc* pNpc, void* pData);	
};
extern KNpcAttribModify g_NpcAttribModify;
#endif
