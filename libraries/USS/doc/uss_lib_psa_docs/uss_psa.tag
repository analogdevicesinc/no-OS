<?xml version='1.0' encoding='UTF-8' standalone='yes' ?>
<tagfile doxygen_version="1.12.0" doxygen_gitid="c73f5d30f9e8b1df5ba15a1d064ff2067cbb8267">
  <compound kind="file">
    <name>psa_mainpage.md</name>
    <path></path>
    <filename>psa__mainpage_8md.html</filename>
  </compound>
  <compound kind="file">
    <name>crypto.h</name>
    <path>C:/JS/ci_ist/workspace/spb_ist/USS/USS_CFS_Release/maxq10xx-sdk/maxq10xx-api/include/psa/</path>
    <filename>crypto_8h.html</filename>
    <includes id="crypto__config__def_8h" name="crypto_config_def.h" local="yes" import="no" module="no" objc="no">crypto_config_def.h</includes>
    <includes id="crypto__errors_8h" name="crypto_errors.h" local="yes" import="no" module="no" objc="no">crypto_errors.h</includes>
    <includes id="crypto__typedefs_8h" name="crypto_typedefs.h" local="yes" import="no" module="no" objc="no">crypto_typedefs.h</includes>
    <includes id="crypto__hash_8h" name="crypto_hash.h" local="yes" import="no" module="no" objc="no">crypto_hash.h</includes>
    <includes id="crypto__sign_8h" name="crypto_sign.h" local="yes" import="no" module="no" objc="no">crypto_sign.h</includes>
    <includes id="key__management_8h" name="key_management.h" local="yes" import="no" module="no" objc="no">key_management.h</includes>
    <includes id="crypto__key__derivation_8h" name="crypto_key_derivation.h" local="yes" import="no" module="no" objc="no">crypto_key_derivation.h</includes>
    <includes id="crypto__cipher_8h" name="crypto_cipher.h" local="yes" import="no" module="no" objc="no">crypto_cipher.h</includes>
    <includes id="crypto__aead_8h" name="crypto_aead.h" local="yes" import="no" module="no" objc="no">crypto_aead.h</includes>
    <includes id="crypto__mac_8h" name="crypto_mac.h" local="yes" import="no" module="no" objc="no">crypto_mac.h</includes>
    <member kind="define">
      <type>#define</type>
      <name>PSA_CRYPTO_API_VERSION_MAJOR</name>
      <anchorfile>crypto_8h.html</anchorfile>
      <anchor>ac31060595fde8f06cb25184ca5689b75</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_CRYPTO_API_VERSION_MINOR</name>
      <anchorfile>crypto_8h.html</anchorfile>
      <anchor>a154ccd72cff6c6c5a3a38c9a7027d3bf</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>psa_status_t</type>
      <name>psa_crypto_init</name>
      <anchorfile>crypto_8h.html</anchorfile>
      <anchor>a2de150803fc2f7dc6101d5af7e921dd9</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>psa_key_attributes_t</type>
      <name>psa_key_attributes_init</name>
      <anchorfile>crypto_8h.html</anchorfile>
      <anchor>a5b4ba3747b8080eb61c475e7f5a4af3a</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>psa_status_t</type>
      <name>psa_get_key_attributes</name>
      <anchorfile>crypto_8h.html</anchorfile>
      <anchor>a8e70d36a3db2ec34425722166f5fa75b</anchor>
      <arglist>(psa_key_id_t key, psa_key_attributes_t *attributes)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>psa_reset_key_attributes</name>
      <anchorfile>crypto_8h.html</anchorfile>
      <anchor>a382b7f11fc6d706645cefea22c3bd458</anchor>
      <arglist>(psa_key_attributes_t *attributes)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>psa_set_key_type</name>
      <anchorfile>crypto_8h.html</anchorfile>
      <anchor>ace9aff37de6473175bb8ed0f7ac3e2e6</anchor>
      <arglist>(psa_key_attributes_t *attributes, psa_key_type_t type)</arglist>
    </member>
    <member kind="function">
      <type>psa_key_type_t</type>
      <name>psa_get_key_type</name>
      <anchorfile>crypto_8h.html</anchorfile>
      <anchor>a1272269ad75c545c7cc0834a43310c0b</anchor>
      <arglist>(const psa_key_attributes_t *attributes)</arglist>
    </member>
    <member kind="function">
      <type>size_t</type>
      <name>psa_get_key_bits</name>
      <anchorfile>crypto_8h.html</anchorfile>
      <anchor>a5fdf96e22d1d317308c2e31795aed335</anchor>
      <arglist>(const psa_key_attributes_t *attributes)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>psa_set_key_bits</name>
      <anchorfile>crypto_8h.html</anchorfile>
      <anchor>a8ecfead3671306ec0351a92050c8f43b</anchor>
      <arglist>(psa_key_attributes_t *attributes, size_t bits)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>psa_set_key_lifetime</name>
      <anchorfile>crypto_8h.html</anchorfile>
      <anchor>adac2f4a1384af8229b087b3efd236fa4</anchor>
      <arglist>(psa_key_attributes_t *attributes, psa_key_lifetime_t lifetime)</arglist>
    </member>
    <member kind="function">
      <type>psa_key_lifetime_t</type>
      <name>psa_get_key_lifetime</name>
      <anchorfile>crypto_8h.html</anchorfile>
      <anchor>a8dc887721761ef3a8ef01d43cb009234</anchor>
      <arglist>(const psa_key_attributes_t *attributes)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>psa_set_key_id</name>
      <anchorfile>crypto_8h.html</anchorfile>
      <anchor>a3316ee2d01889e67f556f052341c94b6</anchor>
      <arglist>(psa_key_attributes_t *attributes, psa_key_id_t id)</arglist>
    </member>
    <member kind="function">
      <type>psa_key_id_t</type>
      <name>psa_get_key_id</name>
      <anchorfile>crypto_8h.html</anchorfile>
      <anchor>a35411e2444691ab9436cd372771d0bd0</anchor>
      <arglist>(const psa_key_attributes_t *attributes)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>psa_set_key_algorithm</name>
      <anchorfile>crypto_8h.html</anchorfile>
      <anchor>a97e6c00e1bab25a34c0b2b263d516beb</anchor>
      <arglist>(psa_key_attributes_t *attributes, psa_algorithm_t alg)</arglist>
    </member>
    <member kind="function">
      <type>psa_algorithm_t</type>
      <name>psa_get_key_algorithm</name>
      <anchorfile>crypto_8h.html</anchorfile>
      <anchor>a2b95d1f53253d69fa9996d5ba1cdd666</anchor>
      <arglist>(const psa_key_attributes_t *attributes)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>psa_set_key_usage_flags</name>
      <anchorfile>crypto_8h.html</anchorfile>
      <anchor>a003f2d1e2b9cb668dee98e0a0dddb228</anchor>
      <arglist>(psa_key_attributes_t *attributes, psa_key_usage_t usage_flags)</arglist>
    </member>
    <member kind="function">
      <type>psa_key_usage_t</type>
      <name>psa_get_key_usage_flags</name>
      <anchorfile>crypto_8h.html</anchorfile>
      <anchor>ad04fc4692e59a7a98f4a3de7706fdf63</anchor>
      <arglist>(const psa_key_attributes_t *attributes)</arglist>
    </member>
    <member kind="function">
      <type>psa_status_t</type>
      <name>psa_import_key</name>
      <anchorfile>crypto_8h.html</anchorfile>
      <anchor>ad088d8ebc854d2f7a5b5c1769bc9ed40</anchor>
      <arglist>(const psa_key_attributes_t *attributes, const uint8_t *data, size_t data_length, psa_key_id_t *key)</arglist>
    </member>
    <member kind="function">
      <type>psa_status_t</type>
      <name>psa_generate_key</name>
      <anchorfile>crypto_8h.html</anchorfile>
      <anchor>a950b4199ae7a0164006bd3d6927d1efe</anchor>
      <arglist>(const psa_key_attributes_t *attributes, psa_key_id_t *key)</arglist>
    </member>
    <member kind="function">
      <type>psa_status_t</type>
      <name>psa_copy_key</name>
      <anchorfile>crypto_8h.html</anchorfile>
      <anchor>a78da90274bd8f8680017a4bb513803a1</anchor>
      <arglist>(psa_key_id_t source_key, const psa_key_attributes_t *attributes, psa_key_id_t *target_key)</arglist>
    </member>
    <member kind="function">
      <type>psa_status_t</type>
      <name>psa_destroy_key</name>
      <anchorfile>crypto_8h.html</anchorfile>
      <anchor>af503cd251eff1a0d6cd7d8e47003b92e</anchor>
      <arglist>(psa_key_id_t key)</arglist>
    </member>
    <member kind="function">
      <type>psa_status_t</type>
      <name>psa_purge_key</name>
      <anchorfile>crypto_8h.html</anchorfile>
      <anchor>a09670055d73cef15422ad1b92113c09f</anchor>
      <arglist>(psa_key_id_t key)</arglist>
    </member>
    <member kind="function">
      <type>psa_status_t</type>
      <name>psa_export_key</name>
      <anchorfile>crypto_8h.html</anchorfile>
      <anchor>a5f2f7c851186c5d1d928be0b6020225c</anchor>
      <arglist>(psa_key_id_t key, uint8_t *data, size_t data_size, size_t *data_length)</arglist>
    </member>
    <member kind="function">
      <type>psa_status_t</type>
      <name>psa_export_public_key</name>
      <anchorfile>crypto_8h.html</anchorfile>
      <anchor>a76970efac4f9418a03bf59dc345eaf88</anchor>
      <arglist>(psa_key_id_t key, uint8_t *data, size_t data_size, size_t *data_length)</arglist>
    </member>
    <member kind="function">
      <type>psa_status_t</type>
      <name>psa_hash_compute</name>
      <anchorfile>crypto_8h.html</anchorfile>
      <anchor>ac69f7f19d96a56c28cf3799d11b12156</anchor>
      <arglist>(psa_algorithm_t alg, const uint8_t *input, size_t input_length, uint8_t *hash, size_t hash_size, size_t *hash_length)</arglist>
    </member>
    <member kind="function">
      <type>psa_status_t</type>
      <name>psa_hash_compare</name>
      <anchorfile>crypto_8h.html</anchorfile>
      <anchor>a0c08f4797bec96b886c8c8d7acc2a553</anchor>
      <arglist>(psa_algorithm_t alg, const uint8_t *input, size_t input_length, const uint8_t *hash, size_t hash_length)</arglist>
    </member>
    <member kind="function">
      <type>psa_hash_operation_t</type>
      <name>psa_hash_operation_init</name>
      <anchorfile>crypto_8h.html</anchorfile>
      <anchor>a826a09991b0251aea5b5e4c879edd2eb</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>psa_status_t</type>
      <name>psa_hash_setup</name>
      <anchorfile>crypto_8h.html</anchorfile>
      <anchor>a8d72896cf70fc4d514c5c6b978912515</anchor>
      <arglist>(psa_hash_operation_t *operation, psa_algorithm_t alg)</arglist>
    </member>
    <member kind="function">
      <type>psa_status_t</type>
      <name>psa_hash_update</name>
      <anchorfile>crypto_8h.html</anchorfile>
      <anchor>a65b16ef97d7f650899b7db4b7d1112ff</anchor>
      <arglist>(psa_hash_operation_t *operation, const uint8_t *input, size_t input_length)</arglist>
    </member>
    <member kind="function">
      <type>psa_status_t</type>
      <name>psa_hash_finish</name>
      <anchorfile>crypto_8h.html</anchorfile>
      <anchor>a4795fd06a0067b0adcd92e9627b8c97e</anchor>
      <arglist>(psa_hash_operation_t *operation, uint8_t *hash, size_t hash_size, size_t *hash_length)</arglist>
    </member>
    <member kind="function">
      <type>psa_status_t</type>
      <name>psa_hash_verify</name>
      <anchorfile>crypto_8h.html</anchorfile>
      <anchor>a7be923c5700c9c70ef77ee9b76d1a5c0</anchor>
      <arglist>(psa_hash_operation_t *operation, const uint8_t *hash, size_t hash_length)</arglist>
    </member>
    <member kind="function">
      <type>psa_status_t</type>
      <name>psa_hash_abort</name>
      <anchorfile>crypto_8h.html</anchorfile>
      <anchor>ab0b4d5f9912a615559497a467b532928</anchor>
      <arglist>(psa_hash_operation_t *operation)</arglist>
    </member>
    <member kind="function">
      <type>psa_status_t</type>
      <name>psa_hash_suspend</name>
      <anchorfile>crypto_8h.html</anchorfile>
      <anchor>a32b41f2790027e066b373552af2c5c60</anchor>
      <arglist>(psa_hash_operation_t *operation, uint8_t *hash_state, size_t hash_state_size, size_t *hash_state_length)</arglist>
    </member>
    <member kind="function">
      <type>psa_status_t</type>
      <name>psa_hash_resume</name>
      <anchorfile>crypto_8h.html</anchorfile>
      <anchor>a70d97824771b5195a23236a135fabcc2</anchor>
      <arglist>(psa_hash_operation_t *operation, const uint8_t *hash_state, size_t hash_state_length)</arglist>
    </member>
    <member kind="function">
      <type>psa_status_t</type>
      <name>psa_hash_clone</name>
      <anchorfile>crypto_8h.html</anchorfile>
      <anchor>a39673348f3302b4646bd780034a5aeda</anchor>
      <arglist>(const psa_hash_operation_t *source_operation, psa_hash_operation_t *target_operation)</arglist>
    </member>
    <member kind="function">
      <type>psa_status_t</type>
      <name>psa_mac_compute</name>
      <anchorfile>crypto_8h.html</anchorfile>
      <anchor>afb4c0e338fdc192ef8d293aa545d94cb</anchor>
      <arglist>(psa_key_id_t key, psa_algorithm_t alg, const uint8_t *input, size_t input_length, uint8_t *mac, size_t mac_size, size_t *mac_length)</arglist>
    </member>
    <member kind="function">
      <type>psa_status_t</type>
      <name>psa_mac_verify</name>
      <anchorfile>crypto_8h.html</anchorfile>
      <anchor>aa5542f4a1c590d76c329125d571cf494</anchor>
      <arglist>(psa_key_id_t key, psa_algorithm_t alg, const uint8_t *input, size_t input_length, const uint8_t *mac, size_t mac_length)</arglist>
    </member>
    <member kind="function">
      <type>psa_mac_operation_t</type>
      <name>psa_mac_operation_init</name>
      <anchorfile>crypto_8h.html</anchorfile>
      <anchor>ae4aed389e7a768993cf7cac972767d15</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>psa_status_t</type>
      <name>psa_mac_sign_setup</name>
      <anchorfile>crypto_8h.html</anchorfile>
      <anchor>a6420e166df3967edf60042e589facd04</anchor>
      <arglist>(psa_mac_operation_t *operation, psa_key_id_t key, psa_algorithm_t alg)</arglist>
    </member>
    <member kind="function">
      <type>psa_status_t</type>
      <name>psa_mac_verify_setup</name>
      <anchorfile>crypto_8h.html</anchorfile>
      <anchor>a715679bbf633be958fa447216127eaf8</anchor>
      <arglist>(psa_mac_operation_t *operation, psa_key_id_t key, psa_algorithm_t alg)</arglist>
    </member>
    <member kind="function">
      <type>psa_status_t</type>
      <name>psa_mac_update</name>
      <anchorfile>crypto_8h.html</anchorfile>
      <anchor>a5560af371497babefe03c9da4e8a1c05</anchor>
      <arglist>(psa_mac_operation_t *operation, const uint8_t *input, size_t input_length)</arglist>
    </member>
    <member kind="function">
      <type>psa_status_t</type>
      <name>psa_mac_sign_finish</name>
      <anchorfile>crypto_8h.html</anchorfile>
      <anchor>ac22bc0125580c96724a09226cfbc97f2</anchor>
      <arglist>(psa_mac_operation_t *operation, uint8_t *mac, size_t mac_size, size_t *mac_length)</arglist>
    </member>
    <member kind="function">
      <type>psa_status_t</type>
      <name>psa_mac_verify_finish</name>
      <anchorfile>crypto_8h.html</anchorfile>
      <anchor>ac92b2930d6728e1be4d011c05d485822</anchor>
      <arglist>(psa_mac_operation_t *operation, const uint8_t *mac, size_t mac_length)</arglist>
    </member>
    <member kind="function">
      <type>psa_status_t</type>
      <name>psa_mac_abort</name>
      <anchorfile>crypto_8h.html</anchorfile>
      <anchor>acd8dd54855ba1bc0a03f104f252884fd</anchor>
      <arglist>(psa_mac_operation_t *operation)</arglist>
    </member>
    <member kind="function">
      <type>psa_status_t</type>
      <name>psa_cipher_encrypt</name>
      <anchorfile>crypto_8h.html</anchorfile>
      <anchor>a0ba1e627bd194914ef9297212dd423f9</anchor>
      <arglist>(psa_key_id_t key, psa_algorithm_t alg, const uint8_t *input, size_t input_length, uint8_t *output, size_t output_size, size_t *output_length)</arglist>
    </member>
    <member kind="function">
      <type>psa_status_t</type>
      <name>psa_cipher_decrypt</name>
      <anchorfile>crypto_8h.html</anchorfile>
      <anchor>a31dd2b33667e1a4aebd4789d789c9c55</anchor>
      <arglist>(psa_key_id_t key, psa_algorithm_t alg, const uint8_t *input, size_t input_length, uint8_t *output, size_t output_size, size_t *output_length)</arglist>
    </member>
    <member kind="function">
      <type>psa_cipher_operation_t</type>
      <name>psa_cipher_operation_init</name>
      <anchorfile>crypto_8h.html</anchorfile>
      <anchor>a53b28da37f3d63aad32ab0d8fab6aa08</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>psa_status_t</type>
      <name>psa_cipher_encrypt_setup</name>
      <anchorfile>crypto_8h.html</anchorfile>
      <anchor>ac696b9ac87eac90d25e08e34d040ff72</anchor>
      <arglist>(psa_cipher_operation_t *operation, psa_key_id_t key, psa_algorithm_t alg)</arglist>
    </member>
    <member kind="function">
      <type>psa_status_t</type>
      <name>psa_cipher_decrypt_setup</name>
      <anchorfile>crypto_8h.html</anchorfile>
      <anchor>a9ea37677a0e9152ff712d2ea0f3096ad</anchor>
      <arglist>(psa_cipher_operation_t *operation, psa_key_id_t key, psa_algorithm_t alg)</arglist>
    </member>
    <member kind="function">
      <type>psa_status_t</type>
      <name>psa_cipher_generate_iv</name>
      <anchorfile>crypto_8h.html</anchorfile>
      <anchor>a29fd7d32a5729226a2f73e7b6487bd8a</anchor>
      <arglist>(psa_cipher_operation_t *operation, uint8_t *iv, size_t iv_size, size_t *iv_length)</arglist>
    </member>
    <member kind="function">
      <type>psa_status_t</type>
      <name>psa_cipher_set_iv</name>
      <anchorfile>crypto_8h.html</anchorfile>
      <anchor>a9caddac1a429a5032d6d4a907fb70ba1</anchor>
      <arglist>(psa_cipher_operation_t *operation, const uint8_t *iv, size_t iv_length)</arglist>
    </member>
    <member kind="function">
      <type>psa_status_t</type>
      <name>psa_cipher_update</name>
      <anchorfile>crypto_8h.html</anchorfile>
      <anchor>ac3ca27ac6682917c48247d01fd96cd0f</anchor>
      <arglist>(psa_cipher_operation_t *operation, const uint8_t *input, size_t input_length, uint8_t *output, size_t output_size, size_t *output_length)</arglist>
    </member>
    <member kind="function">
      <type>psa_status_t</type>
      <name>psa_cipher_finish</name>
      <anchorfile>crypto_8h.html</anchorfile>
      <anchor>a1dcb58b8befe23f8a4d7a1d49c99249b</anchor>
      <arglist>(psa_cipher_operation_t *operation, uint8_t *output, size_t output_size, size_t *output_length)</arglist>
    </member>
    <member kind="function">
      <type>psa_status_t</type>
      <name>psa_cipher_abort</name>
      <anchorfile>crypto_8h.html</anchorfile>
      <anchor>aad482cdca2098bca0620596aaa02eaa4</anchor>
      <arglist>(psa_cipher_operation_t *operation)</arglist>
    </member>
    <member kind="function">
      <type>psa_status_t</type>
      <name>psa_aead_encrypt</name>
      <anchorfile>crypto_8h.html</anchorfile>
      <anchor>a174ed4f8e3f3db72670968050d8db6df</anchor>
      <arglist>(psa_key_id_t key, psa_algorithm_t alg, const uint8_t *nonce, size_t nonce_length, const uint8_t *additional_data, size_t additional_data_length, const uint8_t *plaintext, size_t plaintext_length, uint8_t *ciphertext, size_t ciphertext_size, size_t *ciphertext_length)</arglist>
    </member>
    <member kind="function">
      <type>psa_status_t</type>
      <name>psa_aead_decrypt</name>
      <anchorfile>crypto_8h.html</anchorfile>
      <anchor>abb268fefdd140c2e4d802b01b95a8e7a</anchor>
      <arglist>(psa_key_id_t key, psa_algorithm_t alg, const uint8_t *nonce, size_t nonce_length, const uint8_t *additional_data, size_t additional_data_length, const uint8_t *ciphertext, size_t ciphertext_length, uint8_t *plaintext, size_t plaintext_size, size_t *plaintext_length)</arglist>
    </member>
    <member kind="function">
      <type>psa_aead_operation_t</type>
      <name>psa_aead_operation_init</name>
      <anchorfile>crypto_8h.html</anchorfile>
      <anchor>a464e3bbd19095e13420599dc4cf11a5c</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>psa_status_t</type>
      <name>psa_aead_encrypt_setup</name>
      <anchorfile>crypto_8h.html</anchorfile>
      <anchor>a5497da908443e971fdd60248ad07bdae</anchor>
      <arglist>(psa_aead_operation_t *operation, psa_key_id_t key, psa_algorithm_t alg)</arglist>
    </member>
    <member kind="function">
      <type>psa_status_t</type>
      <name>psa_aead_decrypt_setup</name>
      <anchorfile>crypto_8h.html</anchorfile>
      <anchor>a31af4853ff1f9df38b589f6014e278b8</anchor>
      <arglist>(psa_aead_operation_t *operation, psa_key_id_t key, psa_algorithm_t alg)</arglist>
    </member>
    <member kind="function">
      <type>psa_status_t</type>
      <name>psa_aead_set_lengths</name>
      <anchorfile>crypto_8h.html</anchorfile>
      <anchor>ad3431e28d05002c2a7b0760610176050</anchor>
      <arglist>(psa_aead_operation_t *operation, size_t ad_length, size_t plaintext_length)</arglist>
    </member>
    <member kind="function">
      <type>psa_status_t</type>
      <name>psa_aead_generate_nonce</name>
      <anchorfile>crypto_8h.html</anchorfile>
      <anchor>a5799df1c555efd35970b65be51cb07d1</anchor>
      <arglist>(psa_aead_operation_t *operation, uint8_t *nonce, size_t nonce_size, size_t *nonce_length)</arglist>
    </member>
    <member kind="function">
      <type>psa_status_t</type>
      <name>psa_aead_set_nonce</name>
      <anchorfile>crypto_8h.html</anchorfile>
      <anchor>a59132751a6f843d038924cb217b5e13b</anchor>
      <arglist>(psa_aead_operation_t *operation, const uint8_t *nonce, size_t nonce_length)</arglist>
    </member>
    <member kind="function">
      <type>psa_status_t</type>
      <name>psa_aead_update_ad</name>
      <anchorfile>crypto_8h.html</anchorfile>
      <anchor>a6d0eed03f832e5c9c91cb8adf2882569</anchor>
      <arglist>(psa_aead_operation_t *operation, const uint8_t *input, size_t input_length)</arglist>
    </member>
    <member kind="function">
      <type>psa_status_t</type>
      <name>psa_aead_update</name>
      <anchorfile>crypto_8h.html</anchorfile>
      <anchor>af6d49864951ca42136b4a9b71ea26e5c</anchor>
      <arglist>(psa_aead_operation_t *operation, const uint8_t *input, size_t input_length, uint8_t *output, size_t output_size, size_t *output_length)</arglist>
    </member>
    <member kind="function">
      <type>psa_status_t</type>
      <name>psa_aead_finish</name>
      <anchorfile>crypto_8h.html</anchorfile>
      <anchor>a759791bbe1763b377c3b5447641f1fc8</anchor>
      <arglist>(psa_aead_operation_t *operation, uint8_t *ciphertext, size_t ciphertext_size, size_t *ciphertext_length, uint8_t *tag, size_t tag_size, size_t *tag_length)</arglist>
    </member>
    <member kind="function">
      <type>psa_status_t</type>
      <name>psa_aead_verify</name>
      <anchorfile>crypto_8h.html</anchorfile>
      <anchor>ae0280e2e61a185b893c36d858453f0d0</anchor>
      <arglist>(psa_aead_operation_t *operation, uint8_t *plaintext, size_t plaintext_size, size_t *plaintext_length, const uint8_t *tag, size_t tag_length)</arglist>
    </member>
    <member kind="function">
      <type>psa_status_t</type>
      <name>psa_aead_abort</name>
      <anchorfile>crypto_8h.html</anchorfile>
      <anchor>ae8a5f93d92318c8f592ee9fbb9d36ba0</anchor>
      <arglist>(psa_aead_operation_t *operation)</arglist>
    </member>
    <member kind="function">
      <type>psa_key_derivation_operation_t</type>
      <name>psa_key_derivation_operation_init</name>
      <anchorfile>crypto_8h.html</anchorfile>
      <anchor>ae1808d5eab93ebf8fafc18cb02d899c4</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>psa_status_t</type>
      <name>psa_key_derivation_setup</name>
      <anchorfile>crypto_8h.html</anchorfile>
      <anchor>ac0b6a76e45cceb1862752bf041701859</anchor>
      <arglist>(psa_key_derivation_operation_t *operation, psa_algorithm_t alg)</arglist>
    </member>
    <member kind="function">
      <type>psa_status_t</type>
      <name>psa_key_derivation_get_capacity</name>
      <anchorfile>crypto_8h.html</anchorfile>
      <anchor>a9b0b6769491b25e0592fce0f1c96f842</anchor>
      <arglist>(const psa_key_derivation_operation_t *operation, size_t *capacity)</arglist>
    </member>
    <member kind="function">
      <type>psa_status_t</type>
      <name>psa_key_derivation_set_capacity</name>
      <anchorfile>crypto_8h.html</anchorfile>
      <anchor>a8c26178dc8e4581cb9fbce5650db3279</anchor>
      <arglist>(psa_key_derivation_operation_t *operation, size_t capacity)</arglist>
    </member>
    <member kind="function">
      <type>psa_status_t</type>
      <name>psa_key_derivation_input_bytes</name>
      <anchorfile>crypto_8h.html</anchorfile>
      <anchor>a8fd934dfb0ca45cbf89542ef2a5494c2</anchor>
      <arglist>(psa_key_derivation_operation_t *operation, psa_key_derivation_step_t step, const uint8_t *data, size_t data_length)</arglist>
    </member>
    <member kind="function">
      <type>psa_status_t</type>
      <name>psa_key_derivation_input_integer</name>
      <anchorfile>crypto_8h.html</anchorfile>
      <anchor>ac14f4c2fa41c2c1f6154f8a9c7a5a0cd</anchor>
      <arglist>(psa_key_derivation_operation_t *operation, psa_key_derivation_step_t step, uint64_t value)</arglist>
    </member>
    <member kind="function">
      <type>psa_status_t</type>
      <name>psa_key_derivation_input_key</name>
      <anchorfile>crypto_8h.html</anchorfile>
      <anchor>aa936b9b1f6264beb6e23afd5f87bcccc</anchor>
      <arglist>(psa_key_derivation_operation_t *operation, psa_key_derivation_step_t step, psa_key_id_t key)</arglist>
    </member>
    <member kind="function">
      <type>psa_status_t</type>
      <name>psa_key_derivation_output_bytes</name>
      <anchorfile>crypto_8h.html</anchorfile>
      <anchor>a06b7eb34a2fa88965f68e3d023fa12b9</anchor>
      <arglist>(psa_key_derivation_operation_t *operation, uint8_t *output, size_t output_length)</arglist>
    </member>
    <member kind="function">
      <type>psa_status_t</type>
      <name>psa_key_derivation_output_key</name>
      <anchorfile>crypto_8h.html</anchorfile>
      <anchor>ad286b675f3f8b46fa6cab620de6bead3</anchor>
      <arglist>(const psa_key_attributes_t *attributes, psa_key_derivation_operation_t *operation, psa_key_id_t *key)</arglist>
    </member>
    <member kind="function">
      <type>psa_status_t</type>
      <name>psa_key_derivation_verify_bytes</name>
      <anchorfile>crypto_8h.html</anchorfile>
      <anchor>af01520beb7ba932143ffe733b0795b08</anchor>
      <arglist>(psa_key_derivation_operation_t *operation, const uint8_t *expected_output, size_t output_length)</arglist>
    </member>
    <member kind="function">
      <type>psa_status_t</type>
      <name>psa_key_derivation_verify_key</name>
      <anchorfile>crypto_8h.html</anchorfile>
      <anchor>ac041714e34a94742e8ee006ac7dfea5a</anchor>
      <arglist>(psa_key_derivation_operation_t *operation, psa_key_id_t expected)</arglist>
    </member>
    <member kind="function">
      <type>psa_status_t</type>
      <name>psa_key_derivation_abort</name>
      <anchorfile>crypto_8h.html</anchorfile>
      <anchor>a90fdd2716124d0bd258826184824675f</anchor>
      <arglist>(psa_key_derivation_operation_t *operation)</arglist>
    </member>
    <member kind="function">
      <type>psa_status_t</type>
      <name>psa_sign_message</name>
      <anchorfile>crypto_8h.html</anchorfile>
      <anchor>a295407a9d6fd0d7ddc6aebf6ead4955c</anchor>
      <arglist>(psa_key_id_t key, psa_algorithm_t alg, const uint8_t *input, size_t input_length, uint8_t *signature, size_t signature_size, size_t *signature_length)</arglist>
    </member>
    <member kind="function">
      <type>psa_status_t</type>
      <name>psa_verify_message</name>
      <anchorfile>crypto_8h.html</anchorfile>
      <anchor>a8112e02cd89cc652acb2f45074a7464e</anchor>
      <arglist>(psa_key_id_t key, psa_algorithm_t alg, const uint8_t *input, size_t input_length, const uint8_t *signature, size_t signature_length)</arglist>
    </member>
    <member kind="function">
      <type>psa_status_t</type>
      <name>psa_sign_hash</name>
      <anchorfile>crypto_8h.html</anchorfile>
      <anchor>a429241820edc81d0f262bdf43d3b6523</anchor>
      <arglist>(psa_key_id_t key, psa_algorithm_t alg, const uint8_t *hash, size_t hash_length, uint8_t *signature, size_t signature_size, size_t *signature_length)</arglist>
    </member>
    <member kind="function">
      <type>psa_status_t</type>
      <name>psa_verify_hash</name>
      <anchorfile>crypto_8h.html</anchorfile>
      <anchor>a01b4dea9fdb090219123f5de26bb6716</anchor>
      <arglist>(psa_key_id_t key, psa_algorithm_t alg, const uint8_t *hash, size_t hash_length, const uint8_t *signature, size_t signature_length)</arglist>
    </member>
    <member kind="function">
      <type>psa_status_t</type>
      <name>psa_asymmetric_encrypt</name>
      <anchorfile>crypto_8h.html</anchorfile>
      <anchor>a29f79a5294e91c320ba69c1956ccf506</anchor>
      <arglist>(psa_key_id_t key, psa_algorithm_t alg, const uint8_t *input, size_t input_length, const uint8_t *salt, size_t salt_length, uint8_t *output, size_t output_size, size_t *output_length)</arglist>
    </member>
    <member kind="function">
      <type>psa_status_t</type>
      <name>psa_asymmetric_decrypt</name>
      <anchorfile>crypto_8h.html</anchorfile>
      <anchor>a19a171d35473710c3b78744713f3fa64</anchor>
      <arglist>(psa_key_id_t key, psa_algorithm_t alg, const uint8_t *input, size_t input_length, const uint8_t *salt, size_t salt_length, uint8_t *output, size_t output_size, size_t *output_length)</arglist>
    </member>
    <member kind="function">
      <type>psa_status_t</type>
      <name>psa_raw_key_agreement</name>
      <anchorfile>crypto_8h.html</anchorfile>
      <anchor>a0c87346ea56b79f3dd173adee2abbde8</anchor>
      <arglist>(psa_algorithm_t alg, psa_key_id_t private_key, const uint8_t *peer_key, size_t peer_key_length, uint8_t *output, size_t output_size, size_t *output_length)</arglist>
    </member>
    <member kind="function">
      <type>psa_status_t</type>
      <name>psa_key_derivation_key_agreement</name>
      <anchorfile>crypto_8h.html</anchorfile>
      <anchor>a4da37a87ff9863e2a61eccefd331254a</anchor>
      <arglist>(psa_key_derivation_operation_t *operation, psa_key_derivation_step_t step, psa_key_id_t private_key, const uint8_t *peer_key, size_t peer_key_length)</arglist>
    </member>
    <member kind="function">
      <type>psa_status_t</type>
      <name>psa_generate_random</name>
      <anchorfile>crypto_8h.html</anchorfile>
      <anchor>a1985eae417dfbccedf50d5fff54ea8c5</anchor>
      <arglist>(uint8_t *output, size_t output_size)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>crypto_aead.h</name>
    <path>C:/JS/ci_ist/workspace/spb_ist/USS/USS_CFS_Release/maxq10xx-sdk/maxq10xx-api/include/psa/</path>
    <filename>crypto__aead_8h.html</filename>
    <includes id="crypto__common_8h" name="crypto_common.h" local="yes" import="no" module="no" objc="no">crypto_common.h</includes>
    <includes id="crypto__cipher_8h" name="crypto_cipher.h" local="yes" import="no" module="no" objc="no">crypto_cipher.h</includes>
    <member kind="define">
      <type>#define</type>
      <name>USS_PSA_ALG_AEAD_TAG_LENGTH_MASK</name>
      <anchorfile>crypto__aead_8h.html</anchorfile>
      <anchor>a3a530b498be4f0b78a1eba3ad14677da</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>USS_PSA_ALG_AEAD_AT_LEAST_THIS_LEN_FLAG</name>
      <anchorfile>crypto__aead_8h.html</anchorfile>
      <anchor>aa82a211a613a08d5dffe8d8e742ce67b</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>USS_PSA_AEAD_TAG_LENGTH_OFFSET</name>
      <anchorfile>crypto__aead_8h.html</anchorfile>
      <anchor>adf1b320b66c1a34ebb318df1ef74b485</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_ALG_IS_AEAD</name>
      <anchorfile>crypto__aead_8h.html</anchorfile>
      <anchor>a1d44829d60065eaa4ac9a703e7d6abc8</anchor>
      <arglist>(alg)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_ALG_AEAD_WITH_SHORTENED_TAG</name>
      <anchorfile>crypto__aead_8h.html</anchorfile>
      <anchor>abdec2230cb9187e9974b9068bb06777e</anchor>
      <arglist>(aead_alg, tag_length)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_ALG_AEAD_WITH_DEFAULT_LENGTH_TAG</name>
      <anchorfile>crypto__aead_8h.html</anchorfile>
      <anchor>a4aaf387bd55186a76e126788097cdf45</anchor>
      <arglist>(aead_alg)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_ALG_AEAD_WITH_AT_LEAST_THIS_LENGTH_TAG</name>
      <anchorfile>crypto__aead_8h.html</anchorfile>
      <anchor>a09c63eca9d111ca7581b63c38bda3cd7</anchor>
      <arglist>(aead_alg, min_tag_length)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>USS_PSA_ALG_AEAD_FROM_BLOCK_FLAG</name>
      <anchorfile>crypto__aead_8h.html</anchorfile>
      <anchor>ae436c7da29d6f123a88ed9c57f006592</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>USS_PSA_ALG_AEAD_EQUAL</name>
      <anchorfile>crypto__aead_8h.html</anchorfile>
      <anchor>a65167d8a43b0a998f1b3f9947be0257f</anchor>
      <arglist>(aead_alg_1, aead_alg_2)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_ALG_IS_AEAD_ON_BLOCK_CIPHER</name>
      <anchorfile>crypto__aead_8h.html</anchorfile>
      <anchor>afa4d92f00cf52ad28c08c6cb434665e7</anchor>
      <arglist>(alg)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>USS_PSA_ALG_AEAD_GET_TAG_LENGTH</name>
      <anchorfile>crypto__aead_8h.html</anchorfile>
      <anchor>abd700284e695d8737f2d04b627cafd90</anchor>
      <arglist>(aead_alg)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_AEAD_NONCE_LENGTH</name>
      <anchorfile>crypto__aead_8h.html</anchorfile>
      <anchor>a3dcd505a4e70a63708c05bd8591a4782</anchor>
      <arglist>(key_type, alg)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_AEAD_NONCE_MAX_SIZE</name>
      <anchorfile>crypto__aead_8h.html</anchorfile>
      <anchor>ac2a332765ba4ccfc24935d6f7f48fcc7</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_AEAD_ENCRYPT_OUTPUT_SIZE</name>
      <anchorfile>crypto__aead_8h.html</anchorfile>
      <anchor>a0e0791f41b38a79b1fda63c4cb0eec7a</anchor>
      <arglist>(key_type, alg, plaintext_length)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_AEAD_ENCRYPT_OUTPUT_MAX_SIZE</name>
      <anchorfile>crypto__aead_8h.html</anchorfile>
      <anchor>a627fd15f45e7aab42d0126f1534f24a5</anchor>
      <arglist>(plaintext_length)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_AEAD_DECRYPT_OUTPUT_SIZE</name>
      <anchorfile>crypto__aead_8h.html</anchorfile>
      <anchor>ad33bdee829a3d6c4083cae62d176fa9e</anchor>
      <arglist>(key_type, alg, ciphertext_length)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_AEAD_DECRYPT_OUTPUT_MAX_SIZE</name>
      <anchorfile>crypto__aead_8h.html</anchorfile>
      <anchor>aa7135728f0067ffcf6536e7d6c356aa5</anchor>
      <arglist>(ciphertext_length)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_AEAD_UPDATE_OUTPUT_SIZE</name>
      <anchorfile>crypto__aead_8h.html</anchorfile>
      <anchor>a38c502f21653f1b3708ae8897d1bb01d</anchor>
      <arglist>(key_type, alg, input_length)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_AEAD_UPDATE_OUTPUT_MAX_SIZE</name>
      <anchorfile>crypto__aead_8h.html</anchorfile>
      <anchor>a7bad9346cc8ceaffa4a8e7cb8c4fc9ce</anchor>
      <arglist>(input_length)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_AEAD_FINISH_OUTPUT_SIZE</name>
      <anchorfile>crypto__aead_8h.html</anchorfile>
      <anchor>aeaa3630f4450996638d4c56d651301d3</anchor>
      <arglist>(key_type, alg)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_AEAD_FINISH_OUTPUT_MAX_SIZE</name>
      <anchorfile>crypto__aead_8h.html</anchorfile>
      <anchor>ae2d44f764023b9e7948af9344ecb66ce</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_AEAD_TAG_LENGTH</name>
      <anchorfile>crypto__aead_8h.html</anchorfile>
      <anchor>a9aab63d83591633baa13beff3bdf8b2f</anchor>
      <arglist>(key_type, key_bits, alg)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_AEAD_TAG_MAX_SIZE</name>
      <anchorfile>crypto__aead_8h.html</anchorfile>
      <anchor>ae3b873ea579c278946e6e536b7e513af</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_AEAD_VERIFY_OUTPUT_SIZE</name>
      <anchorfile>crypto__aead_8h.html</anchorfile>
      <anchor>a4ca7886c2fedd17a1327ad90ee1cc480</anchor>
      <arglist>(key_type, alg)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>USS_PSA_ALG_AEAD_IS_WILDCARD</name>
      <anchorfile>crypto__aead_8h.html</anchorfile>
      <anchor>a930e643f47921b8d9b3a137897228cab</anchor>
      <arglist>(alg)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_AEAD_VERIFY_OUTPUT_MAX_SIZE</name>
      <anchorfile>crypto__aead_8h.html</anchorfile>
      <anchor>a19fc08f5f6f33fd0b91152de1643df0c</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_AEAD_OPERATION_INIT</name>
      <anchorfile>crypto__aead_8h.html</anchorfile>
      <anchor>af52e036794c0dc6fbadd93a2b990f366</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>crypto_asymmetric_encrypt.h</name>
    <path>C:/JS/ci_ist/workspace/spb_ist/USS/USS_CFS_Release/maxq10xx-sdk/maxq10xx-api/include/psa/</path>
    <filename>crypto__asymmetric__encrypt_8h.html</filename>
    <member kind="define">
      <type>#define</type>
      <name>PSA_ASYMMETRIC_ENCRYPT_OUTPUT_SIZE</name>
      <anchorfile>crypto__asymmetric__encrypt_8h.html</anchorfile>
      <anchor>a66ba3bd93e5ec52870ccc3848778bad8</anchor>
      <arglist>(key_type, key_bits, alg)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_ASYMMETRIC_ENCRYPT_OUTPUT_MAX_SIZE</name>
      <anchorfile>crypto__asymmetric__encrypt_8h.html</anchorfile>
      <anchor>a1a04450654b66dd9ac2fafb9b5413ae3</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_ASYMMETRIC_DECRYPT_OUTPUT_SIZE</name>
      <anchorfile>crypto__asymmetric__encrypt_8h.html</anchorfile>
      <anchor>a61a246f3eac41989821d982e56fea6c1</anchor>
      <arglist>(key_type, key_bits, alg)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_ASYMMETRIC_DECRYPT_OUTPUT_MAX_SIZE</name>
      <anchorfile>crypto__asymmetric__encrypt_8h.html</anchorfile>
      <anchor>a4b1576b11393e43831232cac4bf75f0c</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>crypto_cipher.h</name>
    <path>C:/JS/ci_ist/workspace/spb_ist/USS/USS_CFS_Release/maxq10xx-sdk/maxq10xx-api/include/psa/</path>
    <filename>crypto__cipher_8h.html</filename>
    <includes id="crypto__common_8h" name="crypto_common.h" local="yes" import="no" module="no" objc="no">crypto_common.h</includes>
    <includes id="crypto__config__def_8h" name="crypto_config_def.h" local="yes" import="no" module="no" objc="no">crypto_config_def.h</includes>
    <member kind="define">
      <type>#define</type>
      <name>PSA_CIPHER_ENCRYPT_OUTPUT_SIZE</name>
      <anchorfile>crypto__cipher_8h.html</anchorfile>
      <anchor>ada02457bee639a10a0c3ddbe1f73fedc</anchor>
      <arglist>(key_type, alg, input_length)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_CIPHER_ENCRYPT_OUTPUT_MAX_SIZE</name>
      <anchorfile>crypto__cipher_8h.html</anchorfile>
      <anchor>a745c06526e5e062a9caf45e6f111f67d</anchor>
      <arglist>(input_length)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_CIPHER_DECRYPT_OUTPUT_SIZE</name>
      <anchorfile>crypto__cipher_8h.html</anchorfile>
      <anchor>a413e5c78d89d0f7a4ccdb047e7238125</anchor>
      <arglist>(key_type, alg, input_length)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_CIPHER_DECRYPT_OUTPUT_MAX_SIZE</name>
      <anchorfile>crypto__cipher_8h.html</anchorfile>
      <anchor>afbcce9b24605d70ead18d9c6de6a5ac7</anchor>
      <arglist>(input_length)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_CIPHER_IV_MAX_SIZE</name>
      <anchorfile>crypto__cipher_8h.html</anchorfile>
      <anchor>ad5630c60148421b334b1bb3962bd4f86</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_CIPHER_IV_LENGTH</name>
      <anchorfile>crypto__cipher_8h.html</anchorfile>
      <anchor>adaeef0383905487313ccbfaf1b73c95e</anchor>
      <arglist>(key_type, alg)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_CIPHER_UPDATE_OUTPUT_SIZE</name>
      <anchorfile>crypto__cipher_8h.html</anchorfile>
      <anchor>acf11f477f1a0f57e3258bc345e9198bc</anchor>
      <arglist>(key_type, alg, input_length)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_CIPHER_UPDATE_OUTPUT_MAX_SIZE</name>
      <anchorfile>crypto__cipher_8h.html</anchorfile>
      <anchor>ab1f6598efd6a7dc56e7ad7e34719eb32</anchor>
      <arglist>(input_length)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_BLOCK_CIPHER_BLOCK_LENGTH</name>
      <anchorfile>crypto__cipher_8h.html</anchorfile>
      <anchor>aeed2ae6f2ea1e2f994562ada459d1f03</anchor>
      <arglist>(type)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_CIPHER_FINISH_OUTPUT_SIZE</name>
      <anchorfile>crypto__cipher_8h.html</anchorfile>
      <anchor>a2d9422f5ca18e6025d41134fe883d697</anchor>
      <arglist>(key_type, alg)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_CIPHER_FINISH_OUTPUT_MAX_SIZE</name>
      <anchorfile>crypto__cipher_8h.html</anchorfile>
      <anchor>ac7980b0ebf97b171493560b06c51e8f5</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_BLOCK_CIPHER_BLOCK_MAX_SIZE</name>
      <anchorfile>crypto__cipher_8h.html</anchorfile>
      <anchor>a615cd3a686a1d2301a6ef1198e55a512</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_CIPHER_OPERATION_INIT</name>
      <anchorfile>crypto__cipher_8h.html</anchorfile>
      <anchor>a2da0541aabf9a4995cf2004e36311919</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_ALG_IS_STREAM_CIPHER</name>
      <anchorfile>crypto__cipher_8h.html</anchorfile>
      <anchor>acfec68e0c6175e02e1b2ebc97df383c0</anchor>
      <arglist>(alg)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>crypto_common.h</name>
    <path>C:/JS/ci_ist/workspace/spb_ist/USS/USS_CFS_Release/maxq10xx-sdk/maxq10xx-api/include/psa/</path>
    <filename>crypto__common_8h.html</filename>
    <includes id="crypto__typedefs_8h" name="crypto_typedefs.h" local="yes" import="no" module="no" objc="no">crypto_typedefs.h</includes>
    <member kind="define">
      <type>#define</type>
      <name>PSA_ALG_NONE</name>
      <anchorfile>crypto__common_8h.html</anchorfile>
      <anchor>a87fb4c311ba365a908ca467ee50f447f</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_ALG_FFDH</name>
      <anchorfile>crypto__common_8h.html</anchorfile>
      <anchor>a0ebbb6f93a05b6511e6f108ffd2d1eb4</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_ALG_ECDH</name>
      <anchorfile>crypto__common_8h.html</anchorfile>
      <anchor>ab2dbcf71b63785e7dd7b54a100edee43</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_ALG_PBKDF2_AES_CMAC_PRF_128</name>
      <anchorfile>crypto__common_8h.html</anchorfile>
      <anchor>ae917f201db1efaaf668edd85ac8fb87f</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_ALG_STREAM_CIPHER</name>
      <anchorfile>crypto__common_8h.html</anchorfile>
      <anchor>ad98c105198f7428f7d1dffcb2cd398cd</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_ALG_CTR</name>
      <anchorfile>crypto__common_8h.html</anchorfile>
      <anchor>ad318309706a769cffdc64e4c7e06b2e9</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_ALG_CFB</name>
      <anchorfile>crypto__common_8h.html</anchorfile>
      <anchor>a0088c933e01d671f263a9a1f177cb5bc</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_ALG_OFB</name>
      <anchorfile>crypto__common_8h.html</anchorfile>
      <anchor>ae96bb421fa634c6fa8f571f0112f1ddb</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_ALG_XTS</name>
      <anchorfile>crypto__common_8h.html</anchorfile>
      <anchor>aa722c0e426a797fd6d99623f59748125</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_ALG_ECB_NO_PADDING</name>
      <anchorfile>crypto__common_8h.html</anchorfile>
      <anchor>ab8f0609cd0f12cccc9c950fd5a81a0e3</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_ALG_CBC_NO_PADDING</name>
      <anchorfile>crypto__common_8h.html</anchorfile>
      <anchor>acb332d72716958880ee7f97d8365ae66</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_ALG_CBC_PKCS7</name>
      <anchorfile>crypto__common_8h.html</anchorfile>
      <anchor>aef50d2e9716eb6d476046608e4e0c78c</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_ALG_PBKDF2_AES_CMAC_PRF_128</name>
      <anchorfile>crypto__common_8h.html</anchorfile>
      <anchor>ae917f201db1efaaf668edd85ac8fb87f</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_ALG_RSA_PKCS1V15_CRYPT</name>
      <anchorfile>crypto__common_8h.html</anchorfile>
      <anchor>a4c540d3abe43fb9abcb94f2bc51acef9</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_ALG_HKDF</name>
      <anchorfile>crypto__common_8h.html</anchorfile>
      <anchor>a32a888fb360e6e25cab8a343772c4a82</anchor>
      <arglist>(hash_alg)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_ALG_HKDF_EXPAND</name>
      <anchorfile>crypto__common_8h.html</anchorfile>
      <anchor>a8cd4f3b42f09bbed3f215ca17565138f</anchor>
      <arglist>(hash_alg)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_ALG_HKDF_EXTRACT</name>
      <anchorfile>crypto__common_8h.html</anchorfile>
      <anchor>aa6ed42995d7714568c2925e2c4d1da83</anchor>
      <arglist>(hash_alg)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_ALG_IS_ASYMMETRIC_ENCRYPTION</name>
      <anchorfile>crypto__common_8h.html</anchorfile>
      <anchor>a41d2ee937d54efd76bd54a97b2ebc08a</anchor>
      <arglist>(alg)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_ALG_IS_CIPHER</name>
      <anchorfile>crypto__common_8h.html</anchorfile>
      <anchor>a1d1a5a402ad89a2e68f12bfb535490eb</anchor>
      <arglist>(alg)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_ALG_IS_ECDH</name>
      <anchorfile>crypto__common_8h.html</anchorfile>
      <anchor>a9d9b6533d2a6bea7bac7ae01facb820d</anchor>
      <arglist>(alg)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_ALG_IS_FFDH</name>
      <anchorfile>crypto__common_8h.html</anchorfile>
      <anchor>aa3cf76164cd9375af4fb8a291078a19e</anchor>
      <arglist>(alg)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_ALG_IS_HKDF</name>
      <anchorfile>crypto__common_8h.html</anchorfile>
      <anchor>a1979d0a76fcee6164cf2e65960f38db2</anchor>
      <arglist>(alg)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_ALG_IS_HKDF_EXPAND</name>
      <anchorfile>crypto__common_8h.html</anchorfile>
      <anchor>af697a4b8e100ce5c8fffbb4366e0615c</anchor>
      <arglist>(alg)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_ALG_IS_HKDF_EXTRACT</name>
      <anchorfile>crypto__common_8h.html</anchorfile>
      <anchor>a04f15d32fce74dc726a2267b4c8bddbe</anchor>
      <arglist>(alg)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_ALG_IS_KEY_AGREEMENT</name>
      <anchorfile>crypto__common_8h.html</anchorfile>
      <anchor>a59753742cb06553bd22751bbef472b6f</anchor>
      <arglist>(alg)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_ALG_IS_KEY_DERIVATION</name>
      <anchorfile>crypto__common_8h.html</anchorfile>
      <anchor>af8b90c648aa53dbd06c236695e300cd0</anchor>
      <arglist>(alg)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_ALG_IS_KEY_DERIVATION_STRETCHING</name>
      <anchorfile>crypto__common_8h.html</anchorfile>
      <anchor>a15266fcb8ef4b3830f51d0b513eee926</anchor>
      <arglist>(alg)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_ALG_IS_PBKDF2_HMAC</name>
      <anchorfile>crypto__common_8h.html</anchorfile>
      <anchor>a61201c116f22f0a2bd1ec206d8efd0bf</anchor>
      <arglist>(alg)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_ALG_IS_RAW_KEY_AGREEMENT</name>
      <anchorfile>crypto__common_8h.html</anchorfile>
      <anchor>aa40ab362ce141ce541d69b2eb1f41438</anchor>
      <arglist>(alg)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_ALG_IS_RSA_OAEP</name>
      <anchorfile>crypto__common_8h.html</anchorfile>
      <anchor>a9a85c05fd5c39ca63bbc47fb0755da39</anchor>
      <arglist>(alg)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_ALG_IS_STREAM_CIPHER</name>
      <anchorfile>crypto__common_8h.html</anchorfile>
      <anchor>acfec68e0c6175e02e1b2ebc97df383c0</anchor>
      <arglist>(alg)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_ALG_IS_TLS12_PRF</name>
      <anchorfile>crypto__common_8h.html</anchorfile>
      <anchor>aa3c18890c50222e5219f40ade8927e66</anchor>
      <arglist>(alg)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_ALG_IS_TLS12_PSK_TO_MS</name>
      <anchorfile>crypto__common_8h.html</anchorfile>
      <anchor>ab25ecc074a93fd11069bedfbba5a287b</anchor>
      <arglist>(alg)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_ALG_IS_WILDCARD</name>
      <anchorfile>crypto__common_8h.html</anchorfile>
      <anchor>acf83d7430e82b97cecb8b26ca6fa1426</anchor>
      <arglist>(alg)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_ALG_KEY_AGREEMENT</name>
      <anchorfile>crypto__common_8h.html</anchorfile>
      <anchor>a78bb81cffb87a635c247725eeb2a2682</anchor>
      <arglist>(ka_alg, kdf_alg)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_ALG_KEY_AGREEMENT_GET_BASE</name>
      <anchorfile>crypto__common_8h.html</anchorfile>
      <anchor>af837c55ba698b488b6e63300e3470abf</anchor>
      <arglist>(alg)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_ALG_KEY_AGREEMENT_GET_KDF</name>
      <anchorfile>crypto__common_8h.html</anchorfile>
      <anchor>a56c1189add62b59e8e6a28a809b57037</anchor>
      <arglist>(alg)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_ALG_PBKDF2_HMAC</name>
      <anchorfile>crypto__common_8h.html</anchorfile>
      <anchor>a67ad00bf6421d52f2905b7ef860d6e99</anchor>
      <arglist>(hash_alg)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_ALG_RSA_OAEP</name>
      <anchorfile>crypto__common_8h.html</anchorfile>
      <anchor>aa1235dc3fdd9839c6c1b1a9857344c76</anchor>
      <arglist>(hash_alg)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_ALG_TLS12_PRF</name>
      <anchorfile>crypto__common_8h.html</anchorfile>
      <anchor>a6d5623c2ccda1d4a84e34351af8382d5</anchor>
      <arglist>(hash_alg)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_ALG_TLS12_PSK_TO_MS</name>
      <anchorfile>crypto__common_8h.html</anchorfile>
      <anchor>a039ec797f15d1635d9b2e09a611f8a68</anchor>
      <arglist>(hash_alg)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>USS_PSA_ALG_MASK</name>
      <anchorfile>crypto__common_8h.html</anchorfile>
      <anchor>afd3a553967bc96b23b87e4e24a367dfd</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>USS_PSA_ALG_TYPE_HASH</name>
      <anchorfile>crypto__common_8h.html</anchorfile>
      <anchor>ae9c74a64521c7b00fd93d9b5263b8155</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>USS_PSA_ALG_TYPE_MAC</name>
      <anchorfile>crypto__common_8h.html</anchorfile>
      <anchor>a14b9ea3e84d3a715aea31488b08b75b7</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>USS_PSA_ALG_TYPE_CIPHER</name>
      <anchorfile>crypto__common_8h.html</anchorfile>
      <anchor>a3434172b122ed656ab0eaf80e963aed6</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>USS_PSA_ALG_TYPE_AEAD</name>
      <anchorfile>crypto__common_8h.html</anchorfile>
      <anchor>a7c57a3f75ab4ffc0534782a0a2f2a3a0</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>USS_PSA_ALG_TYPE_SIGN</name>
      <anchorfile>crypto__common_8h.html</anchorfile>
      <anchor>a97e82f369d65ae46d77d64ad58963231</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>USS_PSA_ALG_TYPE_ASYMMETRIC_ENCRYPTION</name>
      <anchorfile>crypto__common_8h.html</anchorfile>
      <anchor>acbe1cce61079226933e390b0c229573e</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>USS_PSA_ALG_TYPE_KEY_DERIVATION</name>
      <anchorfile>crypto__common_8h.html</anchorfile>
      <anchor>ae842e90d0c38737e651717a7ad9543c2</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>USS_PSA_ALG_TYPE_KEY_AGREEMENT</name>
      <anchorfile>crypto__common_8h.html</anchorfile>
      <anchor>a2bc0d291f42ebc44d71295e946e7257e</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>USS_PSA_ALG_HASH_MASK</name>
      <anchorfile>crypto__common_8h.html</anchorfile>
      <anchor>a8b06c60bcdf0a40ed7cff57bc56c70c9</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>USS_PSA_KEY_TYPE_MASK</name>
      <anchorfile>crypto__common_8h.html</anchorfile>
      <anchor>a86bb99c0147c64d98d29c08b2be08854</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>USS_PSA_KEY_TYPE_RAW_MASK</name>
      <anchorfile>crypto__common_8h.html</anchorfile>
      <anchor>a2bb9994377fe44723d328c3cc74960bc</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>USS_PSA_KEY_TYPE_SYMMETRIC_MASK</name>
      <anchorfile>crypto__common_8h.html</anchorfile>
      <anchor>a9016695ac323fd10decb5be92a960d85</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>USS_PSA_KEY_TYPE_PUBLIC_KEY_MASK</name>
      <anchorfile>crypto__common_8h.html</anchorfile>
      <anchor>a7763d2cd11137cf46249b604f8cb8c2a</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>USS_PSA_KEY_TYPE_KEY_PAIR_MASK</name>
      <anchorfile>crypto__common_8h.html</anchorfile>
      <anchor>a97caa31896c3af7b3b8f8c3b99b65faf</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>USS_PSA_BITS_TO_BYTES</name>
      <anchorfile>crypto__common_8h.html</anchorfile>
      <anchor>aa11e5b792e282e888a492bf5dfca685f</anchor>
      <arglist>(bits)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>USS_PSA_BYTES_TO_BITS</name>
      <anchorfile>crypto__common_8h.html</anchorfile>
      <anchor>a6bb123eba0bc847bf0501d0bb8ebdc58</anchor>
      <arglist>(bytes)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>USS_PSA_ROUND_UP_TO_MULTIPLE</name>
      <anchorfile>crypto__common_8h.html</anchorfile>
      <anchor>adf2f47b64b42de8ccbb72f39c7bbaba3</anchor>
      <arglist>(block_size, length)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>USS_ENCRYPT</name>
      <anchorfile>crypto__common_8h.html</anchorfile>
      <anchor>af0b96e51cb57a3d2f77ffec2c97f5804</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>USS_DECRYPT</name>
      <anchorfile>crypto__common_8h.html</anchorfile>
      <anchor>acf6581989c3d64d8ae1cf91ac38da85f</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>USS_TRUE</name>
      <anchorfile>crypto__common_8h.html</anchorfile>
      <anchor>a2febb44d5ae5a05cdb6ad6ede7ea8fe7</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>USS_FALSE</name>
      <anchorfile>crypto__common_8h.html</anchorfile>
      <anchor>a3269a169193ebcbfc674e8dd13374c83</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>mxq_keytype_id_t</type>
      <name>uss_psa_to_maxq_key_type</name>
      <anchorfile>crypto__common_8h.html</anchorfile>
      <anchor>a1145d7449ac43bb1629f00322a98ee33</anchor>
      <arglist>(psa_key_type_t psa_key_type)</arglist>
    </member>
    <member kind="function">
      <type>mxq_object_prop_t</type>
      <name>uss_psa_to_maxq_key_lifetime</name>
      <anchorfile>crypto__common_8h.html</anchorfile>
      <anchor>a245523fc5592d7df72f0595370c4c2c6</anchor>
      <arglist>(psa_key_lifetime_t psa_key_lifetime)</arglist>
    </member>
    <member kind="function">
      <type>mxq_keyuse_t</type>
      <name>uss_psa_to_maxq_keyuse</name>
      <anchorfile>crypto__common_8h.html</anchorfile>
      <anchor>a951d0c2701107a1c46d886700acc0bac</anchor>
      <arglist>(psa_key_usage_t psa_key_use)</arglist>
    </member>
    <member kind="function">
      <type>mxq_objecttype_t</type>
      <name>uss_psa_to_maxq_object_type</name>
      <anchorfile>crypto__common_8h.html</anchorfile>
      <anchor>a087ea319183ebba3b83e7259920da430</anchor>
      <arglist>(psa_key_type_t psa_key_type)</arglist>
    </member>
    <member kind="function">
      <type>psa_algorithm_t</type>
      <name>uss_maxq_to_psa_algo_id</name>
      <anchorfile>crypto__common_8h.html</anchorfile>
      <anchor>a8299472b91aec45b6053d5f21d586a7f</anchor>
      <arglist>(mxq_algo_id_t mxq_algo_type)</arglist>
    </member>
    <member kind="function">
      <type>psa_key_usage_t</type>
      <name>uss_maxq_to_psa_keyuse</name>
      <anchorfile>crypto__common_8h.html</anchorfile>
      <anchor>a09fe68476ec7e563da884bf10eb3d46a</anchor>
      <arglist>(mxq_keyuse_t mxq_key_use)</arglist>
    </member>
    <member kind="function">
      <type>psa_key_type_t</type>
      <name>uss_maxq_to_psa_key_type</name>
      <anchorfile>crypto__common_8h.html</anchorfile>
      <anchor>aa27d249927c1a1948ef09bf642f06e29</anchor>
      <arglist>(mxq_keytype_id_t mxq_key_type, mxq_keyparam_id_t params, mxq_objecttype_t mxq_object_type)</arglist>
    </member>
    <member kind="function">
      <type>psa_status_t</type>
      <name>uss_to_psa_error_code</name>
      <anchorfile>crypto__common_8h.html</anchorfile>
      <anchor>a26b6230dea08ad1af4360cd6d24b871b</anchor>
      <arglist>(mxq_err_t maxqCode)</arglist>
    </member>
    <member kind="function">
      <type>mxq_algo_id_t</type>
      <name>uss_psa_to_maxq_algo_id</name>
      <anchorfile>crypto__common_8h.html</anchorfile>
      <anchor>a6395efbc108363540729474eae81ab93</anchor>
      <arglist>(psa_algorithm_t psa_algo_type)</arglist>
    </member>
    <member kind="function">
      <type>mxq_algo_id_t</type>
      <name>uss_sign_algo_from_psa</name>
      <anchorfile>crypto__common_8h.html</anchorfile>
      <anchor>a41ff05919383dfddb564699d33f21953</anchor>
      <arglist>(psa_algorithm_t psa_algo, bool isMsg)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>check_key_policy</name>
      <anchorfile>crypto__common_8h.html</anchorfile>
      <anchor>ae797a4ade4285f2ca15a1994a222fddb</anchor>
      <arglist>(psa_algorithm_t psa_algo, psa_key_attributes_t *attr)</arglist>
    </member>
    <member kind="function">
      <type>mxq_u2</type>
      <name>get_u2</name>
      <anchorfile>crypto__common_8h.html</anchorfile>
      <anchor>abab502be9b7137f4d71ea695b6e8ab5f</anchor>
      <arglist>(mxq_u1 *psrc, mxq_length *offset)</arglist>
    </member>
    <member kind="function">
      <type>mxq_u1</type>
      <name>get_u1</name>
      <anchorfile>crypto__common_8h.html</anchorfile>
      <anchor>a38c7d6dc320b37fe8cbb054d4dcbcfae</anchor>
      <arglist>(mxq_u1 *psrc, mxq_length *offset)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>ifCryptoInitDone</name>
      <anchorfile>crypto__common_8h.html</anchorfile>
      <anchor>a8d5641bdd36c663e66203d25034e7011</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>uint8_t</type>
      <name>ct_memcmp</name>
      <anchorfile>crypto__common_8h.html</anchorfile>
      <anchor>ae99ac9494acfc4b11415c67c4a07854f</anchor>
      <arglist>(const void *s1, const void *s2, size_t n)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>check_aead_tag_len</name>
      <anchorfile>crypto__common_8h.html</anchorfile>
      <anchor>a0160269195d668774bfc08a4b08d8327</anchor>
      <arglist>(psa_algorithm_t psa_algo)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>crypto_config_def.h</name>
    <path>C:/JS/ci_ist/workspace/spb_ist/USS/USS_CFS_Release/maxq10xx-sdk/maxq10xx-api/include/psa/</path>
    <filename>crypto__config__def_8h.html</filename>
    <member kind="define">
      <type>#define</type>
      <name>USS_PSA_ECC_MAX_CURVE_BITS</name>
      <anchorfile>crypto__config__def_8h.html</anchorfile>
      <anchor>a54061b711b1ccf2edd4d27f45dfbaa70</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>crypto_errors.h</name>
    <path>C:/JS/ci_ist/workspace/spb_ist/USS/USS_CFS_Release/maxq10xx-sdk/maxq10xx-api/include/psa/</path>
    <filename>crypto__errors_8h.html</filename>
    <member kind="define">
      <type>#define</type>
      <name>PSA_SUCCESS</name>
      <anchorfile>crypto__errors_8h.html</anchorfile>
      <anchor>a4cc859e2c66ca381c7418db3527a65e1</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_ERROR_PROGRAMMER_ERROR</name>
      <anchorfile>crypto__errors_8h.html</anchorfile>
      <anchor>a693325faac24600911429b881563c756</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_ERROR_CONNECTION_REFUSED</name>
      <anchorfile>crypto__errors_8h.html</anchorfile>
      <anchor>aedd5502395e5e5bbbaff97f54d16a5f9</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_ERROR_CONNECTION_BUSY</name>
      <anchorfile>crypto__errors_8h.html</anchorfile>
      <anchor>a47cb994583fbd588e4352bb73ba45425</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_ERROR_GENERIC_ERROR</name>
      <anchorfile>crypto__errors_8h.html</anchorfile>
      <anchor>a8bfafd6baac18ce5d3192d1de256238f</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_ERROR_NOT_PERMITTED</name>
      <anchorfile>crypto__errors_8h.html</anchorfile>
      <anchor>a4d1b8dd8526177a15a210b7afc1accb1</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_ERROR_NOT_SUPPORTED</name>
      <anchorfile>crypto__errors_8h.html</anchorfile>
      <anchor>a1dcc6d130633ed5db8942257581b55dd</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_ERROR_INVALID_ARGUMENT</name>
      <anchorfile>crypto__errors_8h.html</anchorfile>
      <anchor>a798df25a505ebf931f7bec1f80f1f85f</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_ERROR_INVALID_HANDLE</name>
      <anchorfile>crypto__errors_8h.html</anchorfile>
      <anchor>adf22718935657c2c3168c228204085f9</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_ERROR_BAD_STATE</name>
      <anchorfile>crypto__errors_8h.html</anchorfile>
      <anchor>a933d40fa2a591004f2e93aa91e11db84</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_ERROR_BUFFER_TOO_SMALL</name>
      <anchorfile>crypto__errors_8h.html</anchorfile>
      <anchor>a695025f4ec11249aee7ea3d0f65e01c8</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_ERROR_ALREADY_EXISTS</name>
      <anchorfile>crypto__errors_8h.html</anchorfile>
      <anchor>af2b34cc87edc72f3ba90071a08210d20</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_ERROR_DOES_NOT_EXIST</name>
      <anchorfile>crypto__errors_8h.html</anchorfile>
      <anchor>a18646babb2ae6cbde02ea3828bbd9141</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_ERROR_INSUFFICIENT_MEMORY</name>
      <anchorfile>crypto__errors_8h.html</anchorfile>
      <anchor>a91b2ad8a867517a2651f1b076c5216e5</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_ERROR_INSUFFICIENT_STORAGE</name>
      <anchorfile>crypto__errors_8h.html</anchorfile>
      <anchor>a897a45eb206a6f6b7be7ffbe36f0d766</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_ERROR_INSUFFICIENT_DATA</name>
      <anchorfile>crypto__errors_8h.html</anchorfile>
      <anchor>a086ac09f3a555f86e7997beb186f5eef</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_ERROR_SERVICE_FAILURE</name>
      <anchorfile>crypto__errors_8h.html</anchorfile>
      <anchor>a08f877db7840b03583122a4f20529c2b</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_ERROR_COMMUNICATION_FAILURE</name>
      <anchorfile>crypto__errors_8h.html</anchorfile>
      <anchor>a5cdb6948371d49e916106249020ea3f7</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_ERROR_STORAGE_FAILURE</name>
      <anchorfile>crypto__errors_8h.html</anchorfile>
      <anchor>add169a1af2707862b95fb9df91dfc37d</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_ERROR_HARDWARE_FAILURE</name>
      <anchorfile>crypto__errors_8h.html</anchorfile>
      <anchor>a08b10e70fa5ff0b05c631d9f8f6b2c6b</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_ERROR_INSUFFICIENT_ENTROPY</name>
      <anchorfile>crypto__errors_8h.html</anchorfile>
      <anchor>a4deb59fec02297ec5d8b42178323f675</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_ERROR_INVALID_SIGNATURE</name>
      <anchorfile>crypto__errors_8h.html</anchorfile>
      <anchor>a35927f755d232c4766de600f2c49e9f2</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_ERROR_INVALID_PADDING</name>
      <anchorfile>crypto__errors_8h.html</anchorfile>
      <anchor>abe29594edbfb152cf153975b0597ac48</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_ERROR_CORRUPTION_DETECTED</name>
      <anchorfile>crypto__errors_8h.html</anchorfile>
      <anchor>a116f4d4c6a9d9db3397991a8472d8950</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_ERROR_DATA_CORRUPT</name>
      <anchorfile>crypto__errors_8h.html</anchorfile>
      <anchor>a9febb81a44bdeb4c6c42bf4f697b13bf</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_ERROR_DATA_INVALID</name>
      <anchorfile>crypto__errors_8h.html</anchorfile>
      <anchor>acebfbd25a7c2a727694fb57182a66533</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_OPERATION_INCOMPLETE</name>
      <anchorfile>crypto__errors_8h.html</anchorfile>
      <anchor>ac0be2a39bf95ab559b75c7ff616450f0</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>crypto_hash.h</name>
    <path>C:/JS/ci_ist/workspace/spb_ist/USS/USS_CFS_Release/maxq10xx-sdk/maxq10xx-api/include/psa/</path>
    <filename>crypto__hash_8h.html</filename>
    <includes id="crypto__typedefs_8h" name="crypto_typedefs.h" local="yes" import="no" module="no" objc="no">crypto_typedefs.h</includes>
    <includes id="crypto__common_8h" name="crypto_common.h" local="yes" import="no" module="no" objc="no">crypto_common.h</includes>
    <includes id="crypto__config__def_8h" name="crypto_config_def.h" local="yes" import="no" module="no" objc="no">crypto_config_def.h</includes>
    <member kind="define">
      <type>#define</type>
      <name>PSA_ALG_MD5</name>
      <anchorfile>crypto__hash_8h.html</anchorfile>
      <anchor>ade591d9286d23382eb5cec099c84180d</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_ALG_RIPEMD160</name>
      <anchorfile>crypto__hash_8h.html</anchorfile>
      <anchor>a6c5d3a32cda59086f07b85ef007033dd</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_ALG_SHA_1</name>
      <anchorfile>crypto__hash_8h.html</anchorfile>
      <anchor>a3fca4e9f9ad4a1158817d1850dee82e5</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_ALG_SHA_224</name>
      <anchorfile>crypto__hash_8h.html</anchorfile>
      <anchor>a25d6a3244d10a7148fe6b026d1979f7b</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_ALG_SHA_256</name>
      <anchorfile>crypto__hash_8h.html</anchorfile>
      <anchor>a619471f978e13cdd0a1e37145e4bf341</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_ALG_SHA_384</name>
      <anchorfile>crypto__hash_8h.html</anchorfile>
      <anchor>a58af64dd9a86a287e8da9ed7739eead4</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_ALG_SHA_512</name>
      <anchorfile>crypto__hash_8h.html</anchorfile>
      <anchor>afba3ae409f46d3dd7f37a0910660c3e9</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_ALG_SHA_512_224</name>
      <anchorfile>crypto__hash_8h.html</anchorfile>
      <anchor>a3fe2d7c3c80e3186ca78d16a35d5d931</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_ALG_SHA_512_256</name>
      <anchorfile>crypto__hash_8h.html</anchorfile>
      <anchor>a5910b3964c14e9613e8643a45b09c2d4</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_ALG_SHA3_224</name>
      <anchorfile>crypto__hash_8h.html</anchorfile>
      <anchor>a16f5fe34ccce68c2fada1224c054a999</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_ALG_SHA3_256</name>
      <anchorfile>crypto__hash_8h.html</anchorfile>
      <anchor>aace70d9515489bbe3c5e7ac1b7d9155b</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_ALG_SHA3_384</name>
      <anchorfile>crypto__hash_8h.html</anchorfile>
      <anchor>ab0f079257ea75e2acfe2fc3b38c78cd8</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_ALG_SHA3_512</name>
      <anchorfile>crypto__hash_8h.html</anchorfile>
      <anchor>a37e5dbe936dddb155e76f2997de27188</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_ALG_SM3</name>
      <anchorfile>crypto__hash_8h.html</anchorfile>
      <anchor>aa90232ca70c2a6f1d4c59a1b4af16ae5</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_ALG_SHAKE256_512</name>
      <anchorfile>crypto__hash_8h.html</anchorfile>
      <anchor>aec67f81d5b80b765702efb4b3d6e202c</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_ALG_ANY_HASH</name>
      <anchorfile>crypto__hash_8h.html</anchorfile>
      <anchor>aa1288ea8bd397a8a3f5e19e94110f2e4</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_HASH_LENGTH</name>
      <anchorfile>crypto__hash_8h.html</anchorfile>
      <anchor>ae0c23128dc05e9680d2c4c1b292bcc77</anchor>
      <arglist>(alg)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_HASH_BLOCK_LENGTH</name>
      <anchorfile>crypto__hash_8h.html</anchorfile>
      <anchor>ab650de09f5b291b6dedaf15c346a2cfe</anchor>
      <arglist>(alg)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_ALG_IS_HASH</name>
      <anchorfile>crypto__hash_8h.html</anchorfile>
      <anchor>ac9280662bb482590b4b33d1dcd32930f</anchor>
      <arglist>(alg)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_ALG_GET_HASH</name>
      <anchorfile>crypto__hash_8h.html</anchorfile>
      <anchor>a181b0fe4619c1a70c5ef82f28eea09f5</anchor>
      <arglist>(alg)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_HASH_OPERATION_INIT</name>
      <anchorfile>crypto__hash_8h.html</anchorfile>
      <anchor>a6ab7fe8d3500bc2f21be840b4f4f8d1d</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>crypto_key_agreement.h</name>
    <path>C:/JS/ci_ist/workspace/spb_ist/USS/USS_CFS_Release/maxq10xx-sdk/maxq10xx-api/include/psa/</path>
    <filename>crypto__key__agreement_8h.html</filename>
    <member kind="define">
      <type>#define</type>
      <name>PSA_RAW_KEY_AGREEMENT_OUTPUT_SIZE</name>
      <anchorfile>crypto__key__agreement_8h.html</anchorfile>
      <anchor>a189c37b83f1d5f792dc119bc5013a978</anchor>
      <arglist>(key_type, key_bits)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_RAW_KEY_AGREEMENT_OUTPUT_MAX_SIZE</name>
      <anchorfile>crypto__key__agreement_8h.html</anchorfile>
      <anchor>af9612bd2bc7851c937fd6d6f94b4aba0</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_TLS12_PSK_TO_MS_PSK_MAX_SIZE</name>
      <anchorfile>crypto__key__agreement_8h.html</anchorfile>
      <anchor>aea956970b7c2221f5c8d179c02d74d43</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>crypto_key_derivation.h</name>
    <path>C:/JS/ci_ist/workspace/spb_ist/USS/USS_CFS_Release/maxq10xx-sdk/maxq10xx-api/include/psa/</path>
    <filename>crypto__key__derivation_8h.html</filename>
    <member kind="define">
      <type>#define</type>
      <name>PSA_KEY_DERIVATION_INPUT_SECRET</name>
      <anchorfile>crypto__key__derivation_8h.html</anchorfile>
      <anchor>a0ddfbe764baba995c402b1b0ef59392e</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_KEY_DERIVATION_INPUT_OTHER_SECRET</name>
      <anchorfile>crypto__key__derivation_8h.html</anchorfile>
      <anchor>ab21b29b1193a900ebfe94945700f9c54</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_KEY_DERIVATION_INPUT_PASSWORD</name>
      <anchorfile>crypto__key__derivation_8h.html</anchorfile>
      <anchor>ae19de2d7c74759878b4a6677b0acdda7</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_KEY_DERIVATION_INPUT_LABEL</name>
      <anchorfile>crypto__key__derivation_8h.html</anchorfile>
      <anchor>a6489d91aa4a2eb50d28c1209bffe9541</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_KEY_DERIVATION_INPUT_CONTEXT</name>
      <anchorfile>crypto__key__derivation_8h.html</anchorfile>
      <anchor>aa7b817f4977b3a9b1380c9bf61970bd3</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_KEY_DERIVATION_INPUT_SALT</name>
      <anchorfile>crypto__key__derivation_8h.html</anchorfile>
      <anchor>ab62757fb125243562c3947a752470d4a</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_KEY_DERIVATION_INPUT_INFO</name>
      <anchorfile>crypto__key__derivation_8h.html</anchorfile>
      <anchor>acef8df989e09c769233f4b779acb5b7d</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_KEY_DERIVATION_INPUT_SEED</name>
      <anchorfile>crypto__key__derivation_8h.html</anchorfile>
      <anchor>accc41e8b4bd4e34bcb46ad1577709535</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_KEY_DERIVATION_INPUT_COST</name>
      <anchorfile>crypto__key__derivation_8h.html</anchorfile>
      <anchor>a4cc0f85fedc37168ac55117ffdba6cd1</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_KEY_DERIVATION_OPERATION_INIT</name>
      <anchorfile>crypto__key__derivation_8h.html</anchorfile>
      <anchor>a5bdec40d201551558579a635520ae85d</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_KEY_DERIVATION_UNLIMITED_CAPACITY</name>
      <anchorfile>crypto__key__derivation_8h.html</anchorfile>
      <anchor>af80e7d71949ec72039c163b33cc2154d</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>crypto_mac.h</name>
    <path>C:/JS/ci_ist/workspace/spb_ist/USS/USS_CFS_Release/maxq10xx-sdk/maxq10xx-api/include/psa/</path>
    <filename>crypto__mac_8h.html</filename>
    <includes id="crypto__config__def_8h" name="crypto_config_def.h" local="yes" import="no" module="no" objc="no">crypto_config_def.h</includes>
    <includes id="crypto__common_8h" name="crypto_common.h" local="yes" import="no" module="no" objc="no">crypto_common.h</includes>
    <includes id="crypto__typedefs_8h" name="crypto_typedefs.h" local="yes" import="no" module="no" objc="no">crypto_typedefs.h</includes>
    <includes id="crypto__hash_8h" name="crypto_hash.h" local="yes" import="no" module="no" objc="no">crypto_hash.h</includes>
    <includes id="crypto__cipher_8h" name="crypto_cipher.h" local="yes" import="no" module="no" objc="no">crypto_cipher.h</includes>
    <member kind="define">
      <type>#define</type>
      <name>USS_PSA_ALG_HMAC_BASE</name>
      <anchorfile>crypto__mac_8h.html</anchorfile>
      <anchor>a3aa75436926b10a490b2117345836401</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>USS_PSA_ALG_CIPHER_MAC_BASE</name>
      <anchorfile>crypto__mac_8h.html</anchorfile>
      <anchor>a6c3cdfe1b73cdcb93bb0769f7ca168e2</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>USS_PSA_ALG_MAC_SUBCATEGORY_MASK</name>
      <anchorfile>crypto__mac_8h.html</anchorfile>
      <anchor>a53ccb985145370a13e46782f67d45777</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_ALG_HMAC</name>
      <anchorfile>crypto__mac_8h.html</anchorfile>
      <anchor>a70f397425684b3efcde1e0e34c28261f</anchor>
      <arglist>(hash_alg)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_ALG_CBC_MAC</name>
      <anchorfile>crypto__mac_8h.html</anchorfile>
      <anchor>a20bdc755de7b90f6621ccb1e6bb5d9e1</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_ALG_CMAC</name>
      <anchorfile>crypto__mac_8h.html</anchorfile>
      <anchor>a146328a1e0023a02464e232d6ecefdc2</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>USS_PSA_ALG_MAC_TRUNCATION_MASK</name>
      <anchorfile>crypto__mac_8h.html</anchorfile>
      <anchor>a03f85e4cde6618aafe3b09829e2935d7</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>USS_PSA_ALG_MAC_AT_LEAST_THIS_LENGTH_FLAG</name>
      <anchorfile>crypto__mac_8h.html</anchorfile>
      <anchor>adc5dc635ff006cb7f029bafd2d253017</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>USS_PSA_MAC_TRUNCATION_OFFSET</name>
      <anchorfile>crypto__mac_8h.html</anchorfile>
      <anchor>acefbfee70ff20ac398686f09ed55eac5</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_ALG_TRUNCATED_MAC</name>
      <anchorfile>crypto__mac_8h.html</anchorfile>
      <anchor>aba2b3168ff49ec31374ecc8c81ac0665</anchor>
      <arglist>(mac_alg, mac_length)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_ALG_FULL_LENGTH_MAC</name>
      <anchorfile>crypto__mac_8h.html</anchorfile>
      <anchor>a38ccc56b173d00f2058aa6112d378a46</anchor>
      <arglist>(mac_alg)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_ALG_AT_LEAST_THIS_LENGTH_MAC</name>
      <anchorfile>crypto__mac_8h.html</anchorfile>
      <anchor>ae9b6eed00b39ff89a0325c83554295d3</anchor>
      <arglist>(mac_alg, min_mac_length)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_ALG_IS_MAC</name>
      <anchorfile>crypto__mac_8h.html</anchorfile>
      <anchor>aca7aee4c9dde316b3b1a150a26eab776</anchor>
      <arglist>(alg)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_ALG_IS_HMAC</name>
      <anchorfile>crypto__mac_8h.html</anchorfile>
      <anchor>a4a050c3c3cbc6eb96418f18847601c8a</anchor>
      <arglist>(alg)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_ALG_IS_BLOCK_CIPHER_MAC</name>
      <anchorfile>crypto__mac_8h.html</anchorfile>
      <anchor>ae49d1eb601125d65a5c5b252aa45479e</anchor>
      <arglist>(alg)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>USS_PSA_MAC_TRUNCATED_LENGTH</name>
      <anchorfile>crypto__mac_8h.html</anchorfile>
      <anchor>a6f707468883de071a87feafd5ff77336</anchor>
      <arglist>(mac_alg)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>USS_PSA_ALG_HMAC_GET_HASH</name>
      <anchorfile>crypto__mac_8h.html</anchorfile>
      <anchor>a20ff9c127a8389af0c47ec1a6d0d4a01</anchor>
      <arglist>(hmac_alg)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_MAC_LENGTH</name>
      <anchorfile>crypto__mac_8h.html</anchorfile>
      <anchor>a4c1151a9816184287aa5f76e82e16080</anchor>
      <arglist>(key_type, key_bits, alg)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_MAC_OPERATION_INIT</name>
      <anchorfile>crypto__mac_8h.html</anchorfile>
      <anchor>a441b6efc161a4573d06465bd22d9dc2d</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_MAC_MAX_SIZE</name>
      <anchorfile>crypto__mac_8h.html</anchorfile>
      <anchor>a4681cc4f6226883a2160122c562ca682</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>USS_MAC_VERIFY_FLAG</name>
      <anchorfile>crypto__mac_8h.html</anchorfile>
      <anchor>a3cb3be7f54447b004c7ab6de39d0ca5d</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>USS_MAC_COMPUTE_FLAG</name>
      <anchorfile>crypto__mac_8h.html</anchorfile>
      <anchor>aff5f32f6c1057f17b0087ae5f63b777b</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>crypto_sign.h</name>
    <path>C:/JS/ci_ist/workspace/spb_ist/USS/USS_CFS_Release/maxq10xx-sdk/maxq10xx-api/include/psa/</path>
    <filename>crypto__sign_8h.html</filename>
    <includes id="crypto__typedefs_8h" name="crypto_typedefs.h" local="yes" import="no" module="no" objc="no">crypto_typedefs.h</includes>
    <includes id="crypto__common_8h" name="crypto_common.h" local="yes" import="no" module="no" objc="no">crypto_common.h</includes>
    <includes id="crypto__config__def_8h" name="crypto_config_def.h" local="yes" import="no" module="no" objc="no">crypto_config_def.h</includes>
    <includes id="key__management_8h" name="key_management.h" local="yes" import="no" module="no" objc="no">key_management.h</includes>
    <member kind="define">
      <type>#define</type>
      <name>PSA_ALG_SIGN_MASK</name>
      <anchorfile>crypto__sign_8h.html</anchorfile>
      <anchor>aeb69b9e960ff0c8c963e87f97f3b27e7</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_ALG_RSA_PKCS1V15_SIGN_BASE</name>
      <anchorfile>crypto__sign_8h.html</anchorfile>
      <anchor>a819b23c9899e92e9f867c7b2ae8f264c</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_ALG_RSA_PKCS1V15_SIGN</name>
      <anchorfile>crypto__sign_8h.html</anchorfile>
      <anchor>a702ff75385a6ae7d4247033f479439af</anchor>
      <arglist>(hash_alg)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_ALG_RSA_PKCS1V15_SIGN_RAW</name>
      <anchorfile>crypto__sign_8h.html</anchorfile>
      <anchor>a4215e2a78dcf834e9a625927faa2a817</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_ALG_RSA_PSS_BASE</name>
      <anchorfile>crypto__sign_8h.html</anchorfile>
      <anchor>a49d39a343790971b7a74644f4faea0c0</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_ALG_RSA_PSS</name>
      <anchorfile>crypto__sign_8h.html</anchorfile>
      <anchor>a62152bf4cb4bf6aace5e1be8f143564d</anchor>
      <arglist>(hash_alg)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_ALG_RSA_PSS_ANY_SALT_BASE</name>
      <anchorfile>crypto__sign_8h.html</anchorfile>
      <anchor>a52fa2bc06342341aedf60205738dba84</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_ALG_RSA_PSS_ANY_SALT</name>
      <anchorfile>crypto__sign_8h.html</anchorfile>
      <anchor>a9b7355a2cd6bde88177634d539127f2b</anchor>
      <arglist>(hash_alg)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_ALG_ECDSA_BASE</name>
      <anchorfile>crypto__sign_8h.html</anchorfile>
      <anchor>afd9800fdbe6ea881e0ac0ce03d145928</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_ALG_ECDSA</name>
      <anchorfile>crypto__sign_8h.html</anchorfile>
      <anchor>a7e3ce9f514a227d5ba5d8318870452e3</anchor>
      <arglist>(hash_alg)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_ALG_ECDSA_ANY</name>
      <anchorfile>crypto__sign_8h.html</anchorfile>
      <anchor>a51d6b6044a62e33cae0cf64bfc3b22a4</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_ALG_DETERMINISTIC_ECDSA_BASE</name>
      <anchorfile>crypto__sign_8h.html</anchorfile>
      <anchor>a6c08b65200140aeb46ee9db9c8ed878c</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_ALG_DETERMINISTIC_ECDSA</name>
      <anchorfile>crypto__sign_8h.html</anchorfile>
      <anchor>a11da566bcd341661c8de921e2ca5ed03</anchor>
      <arglist>(hash_alg)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_ALG_PURE_EDDSA</name>
      <anchorfile>crypto__sign_8h.html</anchorfile>
      <anchor>a76d6053fb1867c2c46c5f8131c8a8cae</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_ALG_HASH_EDDSA_BASE</name>
      <anchorfile>crypto__sign_8h.html</anchorfile>
      <anchor>a9a689cd47452af6f5d6f71edd4b86d70</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_ALG_ED25519PH</name>
      <anchorfile>crypto__sign_8h.html</anchorfile>
      <anchor>a3be20c4293031a8f32e7cb66d26f5c3d</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_ALG_ED448PH</name>
      <anchorfile>crypto__sign_8h.html</anchorfile>
      <anchor>a1c632df795535af4334d37a08a01481b</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_ALG_IS_SIGN</name>
      <anchorfile>crypto__sign_8h.html</anchorfile>
      <anchor>a6d490d0904e0698f6c1268a89d72ff31</anchor>
      <arglist>(alg)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_ALG_IS_SIGN_MESSAGE</name>
      <anchorfile>crypto__sign_8h.html</anchorfile>
      <anchor>acd8b87e858614134de566edaa4d84db8</anchor>
      <arglist>(alg)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_ALG_IS_RSA_PKCS1V15_SIGN</name>
      <anchorfile>crypto__sign_8h.html</anchorfile>
      <anchor>a9d545279f23d43b1b2a744d0dd6826d0</anchor>
      <arglist>(alg)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_ALG_IS_RSA_PSS_ANY_SALT</name>
      <anchorfile>crypto__sign_8h.html</anchorfile>
      <anchor>ae93883052098a9a433251599be3e05e5</anchor>
      <arglist>(alg)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_ALG_IS_RSA_PSS_STANDARD_SALT</name>
      <anchorfile>crypto__sign_8h.html</anchorfile>
      <anchor>a3577ae16a2f607c3d4f894344258aa18</anchor>
      <arglist>(alg)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_ALG_IS_RSA_PSS</name>
      <anchorfile>crypto__sign_8h.html</anchorfile>
      <anchor>afa04fae7393a76d5161558768cb82a78</anchor>
      <arglist>(alg)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_ALG_ECDSA_DETERMINISTIC_FLAG</name>
      <anchorfile>crypto__sign_8h.html</anchorfile>
      <anchor>a409294c965c3e28728f8cb9822e886d1</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_ALG_IS_ECDSA</name>
      <anchorfile>crypto__sign_8h.html</anchorfile>
      <anchor>afb92dc138c9d2388033ff5fc1dab7b48</anchor>
      <arglist>(alg)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_ALG_IS_DETERMINISTIC_ECDSA</name>
      <anchorfile>crypto__sign_8h.html</anchorfile>
      <anchor>acd8766fe0fb8c1e2d32644e0d092c43a</anchor>
      <arglist>(alg)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_ALG_IS_RANDOMIZED_ECDSA</name>
      <anchorfile>crypto__sign_8h.html</anchorfile>
      <anchor>ae7b0fafebd139f6f815285b7cad622ea</anchor>
      <arglist>(alg)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_ALG_IS_HASH_EDDSA</name>
      <anchorfile>crypto__sign_8h.html</anchorfile>
      <anchor>ada6b4732688e435a881f7771e3a63e16</anchor>
      <arglist>(alg)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_ALG_IS_SIGN_HASH</name>
      <anchorfile>crypto__sign_8h.html</anchorfile>
      <anchor>a7664f69c28b193cc6cb8368481fcda64</anchor>
      <arglist>(alg)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_ALG_IS_HASH_AND_SIGN</name>
      <anchorfile>crypto__sign_8h.html</anchorfile>
      <anchor>ad8a8ea0536975363b66410cdeafe38b6</anchor>
      <arglist>(alg)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>USS_PSA_ECDSA_SIGNATURE_SIZE</name>
      <anchorfile>crypto__sign_8h.html</anchorfile>
      <anchor>ac571e52655fcd73528189a426c9c0bd8</anchor>
      <arglist>(curve_bits)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_SIGN_OUTPUT_SIZE</name>
      <anchorfile>crypto__sign_8h.html</anchorfile>
      <anchor>a31021bbd8bb365fc350532b5fd18a25a</anchor>
      <arglist>(key_type, key_bits, alg)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_SIGNATURE_MAX_SIZE</name>
      <anchorfile>crypto__sign_8h.html</anchorfile>
      <anchor>a687a317642db9486fda38ea553900a1e</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>crypto_typedefs.h</name>
    <path>C:/JS/ci_ist/workspace/spb_ist/USS/USS_CFS_Release/maxq10xx-sdk/maxq10xx-api/include/psa/</path>
    <filename>crypto__typedefs_8h.html</filename>
    <class kind="struct">psa_key_derivation_operation_s</class>
    <class kind="struct">psa_mac_operation_s</class>
    <class kind="struct">psa_uss_hash_t</class>
    <class kind="struct">psa_key_attributes_t</class>
    <class kind="struct">uss_aead_ctx_t</class>
    <class kind="struct">uss_cipher_ctx_t</class>
    <class kind="struct">psa_aead_operation_s</class>
    <class kind="struct">psa_cipher_operation_s</class>
    <member kind="typedef">
      <type>uint32_t</type>
      <name>psa_key_id_t</name>
      <anchorfile>crypto__typedefs_8h.html</anchorfile>
      <anchor>a11e986351c65bd3dc3c0fe2cd9926e4b</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>int32_t</type>
      <name>psa_status_t</name>
      <anchorfile>crypto__typedefs_8h.html</anchorfile>
      <anchor>a05676e70ba5c6a7565aff3c36677c1f9</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>uint32_t</type>
      <name>psa_key_lifetime_t</name>
      <anchorfile>crypto__typedefs_8h.html</anchorfile>
      <anchor>a6821ff6dd39dc2bc370ded760ad8b0cf</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>uint8_t</type>
      <name>psa_key_persistence_t</name>
      <anchorfile>crypto__typedefs_8h.html</anchorfile>
      <anchor>a183689a3018896e1e2e4f2bcc57bcff3</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>uint32_t</type>
      <name>psa_key_location_t</name>
      <anchorfile>crypto__typedefs_8h.html</anchorfile>
      <anchor>a5e700601f686237a8d1e29363ae543d5</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>uint16_t</type>
      <name>psa_key_type_t</name>
      <anchorfile>crypto__typedefs_8h.html</anchorfile>
      <anchor>a63fce6880ca5933b5d6baa257febf1f6</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>uint8_t</type>
      <name>psa_ecc_family_t</name>
      <anchorfile>crypto__typedefs_8h.html</anchorfile>
      <anchor>ae7ad3569cb9c91b1ebe59a056edb2fc6</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>uint8_t</type>
      <name>psa_dh_family_t</name>
      <anchorfile>crypto__typedefs_8h.html</anchorfile>
      <anchor>af632c5b13aa19c98f8591d4beda8dd1a</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>uint32_t</type>
      <name>psa_key_usage_t</name>
      <anchorfile>crypto__typedefs_8h.html</anchorfile>
      <anchor>a7bb9de71337e0e98de843aa7f9b55f25</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>uint32_t</type>
      <name>psa_algorithm_t</name>
      <anchorfile>crypto__typedefs_8h.html</anchorfile>
      <anchor>ac2e4d47f1300d73c2f829a6d99252d69</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>uint16_t</type>
      <name>psa_key_derivation_step_t</name>
      <anchorfile>crypto__typedefs_8h.html</anchorfile>
      <anchor>aac4eeacd36596c548b3a48fc06c5048b</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>psa_key_derivation_operation_s</type>
      <name>psa_key_derivation_operation_t</name>
      <anchorfile>crypto__typedefs_8h.html</anchorfile>
      <anchor>aff8121df989d9a03cd8987845031111c</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>psa_mac_operation_s</type>
      <name>psa_mac_operation_t</name>
      <anchorfile>crypto__typedefs_8h.html</anchorfile>
      <anchor>a20738279df51a3977b974af686513b61</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>psa_uss_hash_t</type>
      <name>psa_hash_operation_t</name>
      <anchorfile>crypto__typedefs_8h.html</anchorfile>
      <anchor>a4129eb40bd8b5100b6c6886b86a4ea4e</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>psa_aead_operation_s</type>
      <name>psa_aead_operation_t</name>
      <anchorfile>crypto__typedefs_8h.html</anchorfile>
      <anchor>af481abcdf684845656cd130368d07fc9</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>psa_cipher_operation_s</type>
      <name>psa_cipher_operation_t</name>
      <anchorfile>crypto__typedefs_8h.html</anchorfile>
      <anchor>ad7990e8fc7f2f225a3f4630e79db5332</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>examples.h</name>
    <path>C:/JS/ci_ist/workspace/spb_ist/USS/USS_CFS_Release/maxq10xx-sdk/maxq10xx-api/include/psa/</path>
    <filename>examples_8h.html</filename>
    <member kind="define">
      <type>#define</type>
      <name>TEST_FAILED</name>
      <anchorfile>examples_8h.html</anchorfile>
      <anchor>a8b8b91205df891e2c7837bd03795306f</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ASSERTSUCCESS</name>
      <anchorfile>examples_8h.html</anchorfile>
      <anchor>abbb6f6ce3335f1be149c668d3d387dca</anchor>
      <arglist>(fn)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ASSERTERROR</name>
      <anchorfile>examples_8h.html</anchorfile>
      <anchor>a1044101210e75e9ad4d2770b0c7b5638</anchor>
      <arglist>(fn, status)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ASSERTTRUE</name>
      <anchorfile>examples_8h.html</anchorfile>
      <anchor>aa431291f6e5231c09389a1e274910267</anchor>
      <arglist>(x)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>EXITPROG</name>
      <anchorfile>examples_8h.html</anchorfile>
      <anchor>aff42f689d36068dcf28d788ae0d56c4d</anchor>
      <arglist>(err)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>KEYCOMPLEN</name>
      <anchorfile>examples_8h.html</anchorfile>
      <anchor>a6c843b922e1ba029c0c70b4a6b2f400b</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PUBKEYADMINID</name>
      <anchorfile>examples_8h.html</anchorfile>
      <anchor>a667f5d038b336193a1c6dca02dd5b3c8</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PUBKEYIMPORTOBJID</name>
      <anchorfile>examples_8h.html</anchorfile>
      <anchor>ab3732f552c5a9d6ba263970ace1b9044</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>KEYCOMPLEN</name>
      <anchorfile>examples_8h.html</anchorfile>
      <anchor>a6c843b922e1ba029c0c70b4a6b2f400b</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PUBKEYADMINID</name>
      <anchorfile>examples_8h.html</anchorfile>
      <anchor>a667f5d038b336193a1c6dca02dd5b3c8</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PUBKEYIMPORTOBJID</name>
      <anchorfile>examples_8h.html</anchorfile>
      <anchor>ab3732f552c5a9d6ba263970ace1b9044</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>Administrator_Authentication</name>
      <anchorfile>examples_8h.html</anchorfile>
      <anchor>a515dd21f58a593a365c19740b37d4ebb</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>ECDSA_sign_secp256r1_sha256</name>
      <anchorfile>examples_8h.html</anchorfile>
      <anchor>a1f4ea431e8f2ba7d4231cbf53de17023</anchor>
      <arglist>(mxq_u1 *dest, mxq_u1 *key, mxq_u1 *data, mxq_length data_length)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>print_buffer</name>
      <anchorfile>examples_8h.html</anchorfile>
      <anchor>a3b4e00b63472d775fa72215579e75d12</anchor>
      <arglist>(const unsigned char *buffer, unsigned int len)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>mxq_perror</name>
      <anchorfile>examples_8h.html</anchorfile>
      <anchor>a03f5ae65d95867d12f12c50771bf0f2d</anchor>
      <arglist>(mxq_u2 error_code, char *file, int line)</arglist>
    </member>
    <member kind="variable">
      <type>mxq_u1</type>
      <name>KeyPairAdmin</name>
      <anchorfile>examples_8h.html</anchorfile>
      <anchor>a9845bb697a44624fc8dcf85a600c542d</anchor>
      <arglist>[96]</arglist>
    </member>
    <member kind="variable">
      <type>mxq_u1</type>
      <name>KeyPairImport</name>
      <anchorfile>examples_8h.html</anchorfile>
      <anchor>a085bb0466e73f8c0cc831d7eeb6872bb</anchor>
      <arglist>[96]</arglist>
    </member>
    <member kind="variable">
      <type>mxq_u1</type>
      <name>KeySecureChannel</name>
      <anchorfile>examples_8h.html</anchorfile>
      <anchor>a19bce917271f371dc3524d4fd4762369</anchor>
      <arglist>[32]</arglist>
    </member>
    <member kind="variable">
      <type>mxq_u1</type>
      <name>KeyPairSecureBoot</name>
      <anchorfile>examples_8h.html</anchorfile>
      <anchor>aecabad3ff0c05e1e053f790faebb9ca6</anchor>
      <arglist>[96]</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>key_management.h</name>
    <path>C:/JS/ci_ist/workspace/spb_ist/USS/USS_CFS_Release/maxq10xx-sdk/maxq10xx-api/include/psa/</path>
    <filename>key__management_8h.html</filename>
    <includes id="crypto__config__def_8h" name="crypto_config_def.h" local="yes" import="no" module="no" objc="no">psa/crypto_config_def.h</includes>
    <includes id="crypto__typedefs_8h" name="crypto_typedefs.h" local="yes" import="no" module="no" objc="no">psa/crypto_typedefs.h</includes>
    <member kind="define">
      <type>#define</type>
      <name>PSA_KEY_ATTRIBUTES_INIT</name>
      <anchorfile>key__management_8h.html</anchorfile>
      <anchor>ac45b6c1e7228a1f425338279ee679c4e</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_KEY_TYPE_NONE</name>
      <anchorfile>key__management_8h.html</anchorfile>
      <anchor>afce7ab2b54ce97ea5bff73f13a9f3e5b</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_KEY_TYPE_RAW_DATA</name>
      <anchorfile>key__management_8h.html</anchorfile>
      <anchor>aa97f92025533102616b32d571c940d80</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_KEY_TYPE_HMAC</name>
      <anchorfile>key__management_8h.html</anchorfile>
      <anchor>a92d989f4ca64abd00f463defd773a6f8</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_KEY_TYPE_DERIVE</name>
      <anchorfile>key__management_8h.html</anchorfile>
      <anchor>ae871b2357b8593f33bfd51abbf93ebb1</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_KEY_TYPE_PASSWORD</name>
      <anchorfile>key__management_8h.html</anchorfile>
      <anchor>aad682c8fe975aee537af922f9503d510</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_KEY_TYPE_PASSWORD_HASH</name>
      <anchorfile>key__management_8h.html</anchorfile>
      <anchor>ac41a0d5fdafc1c19ae73313613142723</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_KEY_TYPE_PEPPER</name>
      <anchorfile>key__management_8h.html</anchorfile>
      <anchor>ab47765c3f82af1b6a7d213f9881cb6d6</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_KEY_TYPE_AES</name>
      <anchorfile>key__management_8h.html</anchorfile>
      <anchor>a6ee54579dcf278c677eda4bb1a29575e</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_KEY_TYPE_ARIA</name>
      <anchorfile>key__management_8h.html</anchorfile>
      <anchor>a853d69f982740f4fd40cc964075c188f</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_KEY_TYPE_DES</name>
      <anchorfile>key__management_8h.html</anchorfile>
      <anchor>a577562bfbbc691c820d55ec308333138</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_KEY_TYPE_CAMELLIA</name>
      <anchorfile>key__management_8h.html</anchorfile>
      <anchor>ad8e5da742343fd5519f9d8a630c2ed81</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_KEY_TYPE_SM4</name>
      <anchorfile>key__management_8h.html</anchorfile>
      <anchor>a5b2b1f1434710f4ddbcea30919f08a2c</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_KEY_TYPE_ARC4</name>
      <anchorfile>key__management_8h.html</anchorfile>
      <anchor>ae4d46e83f910dcaa126000a8ed03cde9</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_KEY_TYPE_CHACHA20</name>
      <anchorfile>key__management_8h.html</anchorfile>
      <anchor>a901548883b3bce56cc21c3a22cf8d93c</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_KEY_TYPE_RSA_KEY_PAIR</name>
      <anchorfile>key__management_8h.html</anchorfile>
      <anchor>a11745b110166e927e2abeabc7d532051</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_KEY_TYPE_RSA_PUBLIC_KEY</name>
      <anchorfile>key__management_8h.html</anchorfile>
      <anchor>a9ba0878f56c8bcd1995ac017a74f513b</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_ECC_FAMILY_SECP_K1</name>
      <anchorfile>key__management_8h.html</anchorfile>
      <anchor>a5cf2405c6e23b93cea0aac092d1aa0f7</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_ECC_FAMILY_SECP_R1</name>
      <anchorfile>key__management_8h.html</anchorfile>
      <anchor>a48bb340b5544ba617b0f5b89542665a7</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_ECC_FAMILY_SECP_R2</name>
      <anchorfile>key__management_8h.html</anchorfile>
      <anchor>a95a662f6fa5a11f0b7b7050e24acf147</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_ECC_FAMILY_SECT_K1</name>
      <anchorfile>key__management_8h.html</anchorfile>
      <anchor>a31fb7fe7608708fa64bcec2eea92efbb</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_ECC_FAMILY_SECT_R1</name>
      <anchorfile>key__management_8h.html</anchorfile>
      <anchor>a71544d130f52ad2afc7700779c44af19</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_ECC_FAMILY_SECT_R2</name>
      <anchorfile>key__management_8h.html</anchorfile>
      <anchor>a40d07f0ac30b4bc7c93d65408ca31d00</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_ECC_FAMILY_BRAINPOOL_P_R1</name>
      <anchorfile>key__management_8h.html</anchorfile>
      <anchor>ac1643f1baf38b30d07c20a6eac697f15</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_ECC_FAMILY_FRP</name>
      <anchorfile>key__management_8h.html</anchorfile>
      <anchor>aaf7310c0eaed708f3199796d95b70633</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_ECC_FAMILY_MONTGOMERY</name>
      <anchorfile>key__management_8h.html</anchorfile>
      <anchor>a1f624c5cdaf25b21287af33024e1aff8</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_ECC_FAMILY_TWISTED_EDWARDS</name>
      <anchorfile>key__management_8h.html</anchorfile>
      <anchor>afe45f9fe29699f26795f6251f47eafd4</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_KEY_TYPE_DH_GET_FAMILY</name>
      <anchorfile>key__management_8h.html</anchorfile>
      <anchor>a16c2e7d4bca4ed4587fb59002f14ddf5</anchor>
      <arglist>(type)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_KEY_TYPE_DH_KEY_PAIR</name>
      <anchorfile>key__management_8h.html</anchorfile>
      <anchor>ab4f857c4cd56f5fe65ded421e61bcc8c</anchor>
      <arglist>(group)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_KEY_TYPE_DH_PUBLIC_KEY</name>
      <anchorfile>key__management_8h.html</anchorfile>
      <anchor>aa22f0f2ea89b929f2fadc19890cc5d5c</anchor>
      <arglist>(group)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_KEY_TYPE_ECC_GET_FAMILY</name>
      <anchorfile>key__management_8h.html</anchorfile>
      <anchor>a30642b7e640a6e9e00871764afe0288d</anchor>
      <arglist>(type)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_KEY_TYPE_ECC_KEY_PAIR</name>
      <anchorfile>key__management_8h.html</anchorfile>
      <anchor>a0b6f5d4d5037c54ffa850d8059c32df0</anchor>
      <arglist>(curve)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_KEY_TYPE_ECC_PUBLIC_KEY</name>
      <anchorfile>key__management_8h.html</anchorfile>
      <anchor>ad54c03d3b47020e571a72cd01d978cf2</anchor>
      <arglist>(curve)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_KEY_TYPE_IS_ASYMMETRIC</name>
      <anchorfile>key__management_8h.html</anchorfile>
      <anchor>ab138ae2ebf2905dfbaf4154db2620939</anchor>
      <arglist>(type)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_KEY_TYPE_IS_DH</name>
      <anchorfile>key__management_8h.html</anchorfile>
      <anchor>a248ae35c0e2becaebbf479fc1c3a3b0e</anchor>
      <arglist>(type)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_KEY_TYPE_IS_DH_KEY_PAIR</name>
      <anchorfile>key__management_8h.html</anchorfile>
      <anchor>abb9dd614786fe328df4e485c1a61ed25</anchor>
      <arglist>(type)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_KEY_TYPE_IS_DH_PUBLIC_KEY</name>
      <anchorfile>key__management_8h.html</anchorfile>
      <anchor>a23627e047fa4b536f07d6f4796f879f5</anchor>
      <arglist>(type)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_KEY_TYPE_IS_ECC</name>
      <anchorfile>key__management_8h.html</anchorfile>
      <anchor>a88e01fa06b585654689a99bcc06bbe66</anchor>
      <arglist>(type)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_KEY_TYPE_IS_ECC_KEY_PAIR</name>
      <anchorfile>key__management_8h.html</anchorfile>
      <anchor>ab00289ec3d84a469950a4d8c10fb8cc6</anchor>
      <arglist>(type)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_KEY_TYPE_IS_ECC_PUBLIC_KEY</name>
      <anchorfile>key__management_8h.html</anchorfile>
      <anchor>a5af146a173b0c84d7e737e2fb6a3c0a7</anchor>
      <arglist>(type)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_KEY_TYPE_IS_KEY_PAIR</name>
      <anchorfile>key__management_8h.html</anchorfile>
      <anchor>ae977b8659473ca2ce7b93a01e9596581</anchor>
      <arglist>(type)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_KEY_TYPE_IS_PUBLIC_KEY</name>
      <anchorfile>key__management_8h.html</anchorfile>
      <anchor>ac674a0f059bc0cb72b47f0c517b4f45b</anchor>
      <arglist>(type)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_KEY_TYPE_IS_RSA</name>
      <anchorfile>key__management_8h.html</anchorfile>
      <anchor>a0e1d8f241228e49c9cadadfb4579ef1a</anchor>
      <arglist>(type)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_KEY_TYPE_IS_UNSTRUCTURED</name>
      <anchorfile>key__management_8h.html</anchorfile>
      <anchor>aef86ce4e810e1c2c76068ac874bfef54</anchor>
      <arglist>(type)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_KEY_TYPE_KEY_PAIR_OF_PUBLIC_KEY</name>
      <anchorfile>key__management_8h.html</anchorfile>
      <anchor>adac23f233c110155ba2f0eb1ec1fe055</anchor>
      <arglist>(type)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_KEY_TYPE_PUBLIC_KEY_OF_KEY_PAIR</name>
      <anchorfile>key__management_8h.html</anchorfile>
      <anchor>a679be3d789cfa0c1fd8418764ff754b1</anchor>
      <arglist>(type)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_KEY_LIFETIME_VOLATILE</name>
      <anchorfile>key__management_8h.html</anchorfile>
      <anchor>a8b438870ba69489b685730d346455108</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_KEY_LIFETIME_PERSISTENT</name>
      <anchorfile>key__management_8h.html</anchorfile>
      <anchor>a3713a01c5fcd5f7eae46ff22ceaf6d02</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_KEY_PERSISTENCE_VOLATILE</name>
      <anchorfile>key__management_8h.html</anchorfile>
      <anchor>af3d1317436d19a15e7f2be1b9827edfc</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_KEY_PERSISTENCE_DEFAULT</name>
      <anchorfile>key__management_8h.html</anchorfile>
      <anchor>afc5d5c94f98e885ae5a2bb789deb4d91</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_KEY_PERSISTENCE_READ_ONLY</name>
      <anchorfile>key__management_8h.html</anchorfile>
      <anchor>a6be0f97ba7fb86f626b3fd2b03a1ac79</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_KEY_LOCATION_LOCAL_STORAGE</name>
      <anchorfile>key__management_8h.html</anchorfile>
      <anchor>a3fbfe4a4cf7430a51cf740fe05b2ab27</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_KEY_LOCATION_PRIMARY_SECURE_ELEMENT</name>
      <anchorfile>key__management_8h.html</anchorfile>
      <anchor>a7159eb4b29fe3459a99e005efb32f38d</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_KEY_LIFETIME_GET_PERSISTENCE</name>
      <anchorfile>key__management_8h.html</anchorfile>
      <anchor>a1679b7e418ba86b312291d689153dd2a</anchor>
      <arglist>(lifetime)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_KEY_LIFETIME_GET_LOCATION</name>
      <anchorfile>key__management_8h.html</anchorfile>
      <anchor>a4bd83feb0ca4853eee26d5394e69a5de</anchor>
      <arglist>(lifetime)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_KEY_LIFETIME_IS_VOLATILE</name>
      <anchorfile>key__management_8h.html</anchorfile>
      <anchor>a63d63adbfc947cab4dee4f4b39e840a1</anchor>
      <arglist>(lifetime)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_KEY_LIFETIME_FROM_PERSISTENCE_AND_LOCATION</name>
      <anchorfile>key__management_8h.html</anchorfile>
      <anchor>aef4d244b4d33ae7876ed87226b8112aa</anchor>
      <arglist>(persistence, location)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>USS_PSA_KEY_LIFETIME_WIPE_ON_TAMPER</name>
      <anchorfile>key__management_8h.html</anchorfile>
      <anchor>a46f37dc927d5d5ffdf22f7060f25b9d3</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_KEY_ID_NULL</name>
      <anchorfile>key__management_8h.html</anchorfile>
      <anchor>a650b5f7c553bf6f8db2076dbd9f4dd44</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_KEY_ID_USER_MIN</name>
      <anchorfile>key__management_8h.html</anchorfile>
      <anchor>a2b062011d930839695577138858ec136</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_KEY_ID_USER_MAX</name>
      <anchorfile>key__management_8h.html</anchorfile>
      <anchor>a9cb2bf7c6f7abfacccff4fd31dcfd564</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_KEY_ID_VENDOR_MIN</name>
      <anchorfile>key__management_8h.html</anchorfile>
      <anchor>a1f7e6d90f523bef964d207c1d3236e0b</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_KEY_ID_VENDOR_MAX</name>
      <anchorfile>key__management_8h.html</anchorfile>
      <anchor>a3afb6bb4644c995cf95309a71e7baeb1</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_KEY_USAGE_EXPORT</name>
      <anchorfile>key__management_8h.html</anchorfile>
      <anchor>a7dddccdd1303176e87a4d20c87b589ed</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_KEY_USAGE_COPY</name>
      <anchorfile>key__management_8h.html</anchorfile>
      <anchor>a4605d686020a6cb433c5d78bf7e91689</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_KEY_USAGE_CACHE</name>
      <anchorfile>key__management_8h.html</anchorfile>
      <anchor>ad0babcff38bd6afb43461197266fe073</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_KEY_USAGE_ENCRYPT</name>
      <anchorfile>key__management_8h.html</anchorfile>
      <anchor>a75153b296d045d529d97203a6a995dad</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_KEY_USAGE_DECRYPT</name>
      <anchorfile>key__management_8h.html</anchorfile>
      <anchor>ac3f2d2e5983db1edde9f142ca9bf8e6a</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_KEY_USAGE_SIGN_MESSAGE</name>
      <anchorfile>key__management_8h.html</anchorfile>
      <anchor>a552117ac92b79500cae87d4e65a85c54</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_KEY_USAGE_VERIFY_MESSAGE</name>
      <anchorfile>key__management_8h.html</anchorfile>
      <anchor>abea7ec4173f4f943110329ac2953b2b1</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_KEY_USAGE_SIGN_HASH</name>
      <anchorfile>key__management_8h.html</anchorfile>
      <anchor>a181c73e00d9e95aa3a8366f26c4380b6</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_KEY_USAGE_VERIFY_HASH</name>
      <anchorfile>key__management_8h.html</anchorfile>
      <anchor>afadf131ef2182045e3483d03aadaa1bd</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_KEY_USAGE_DERIVE</name>
      <anchorfile>key__management_8h.html</anchorfile>
      <anchor>af19022acc5ef23cf12477f632b48a0b2</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_KEY_USAGE_VERIFY_DERIVATION</name>
      <anchorfile>key__management_8h.html</anchorfile>
      <anchor>a25aece70be2b79afbdb4180f7c4e68ed</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_EXPORT_KEY_OUTPUT_SIZE</name>
      <anchorfile>key__management_8h.html</anchorfile>
      <anchor>a2e3ff71b9f4e639c03c9e55c7774dc51</anchor>
      <arglist>(key_type, key_bits)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_EXPORT_PUBLIC_KEY_OUTPUT_SIZE</name>
      <anchorfile>key__management_8h.html</anchorfile>
      <anchor>adf475af5fd8eae67bc9cf524d38209f2</anchor>
      <arglist>(key_type, key_bits)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_EXPORT_KEY_PAIR_MAX_SIZE</name>
      <anchorfile>key__management_8h.html</anchorfile>
      <anchor>a5aa21302f87f1e932c9a1daa092df7db</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_EXPORT_PUBLIC_KEY_MAX_SIZE</name>
      <anchorfile>key__management_8h.html</anchorfile>
      <anchor>ad4463cb6edc2061338a88d1adc92b94c</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>psa_key_id_t</type>
      <name>generate_key_id</name>
      <anchorfile>key__management_8h.html</anchorfile>
      <anchor>a478f298a3abd30e87fc0bf4c7547eb35</anchor>
      <arglist>(void)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>psa_name_mangling.h</name>
    <path>C:/JS/ci_ist/workspace/spb_ist/USS/USS_CFS_Release/maxq10xx-sdk/maxq10xx-api/include/psa/</path>
    <filename>psa__name__mangling_8h.html</filename>
    <member kind="define">
      <type>#define</type>
      <name>PSA_NAME_MANGLING_H</name>
      <anchorfile>psa__name__mangling_8h.html</anchorfile>
      <anchor>a23e7d500d4c8da60a608323ec13fb0cf</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PSA_FUNCTION_NAME</name>
      <anchorfile>psa__name__mangling_8h.html</anchorfile>
      <anchor>a792448b61a20377e803abbf47e74109c</anchor>
      <arglist>(x)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>psa_crypto_init</name>
      <anchorfile>psa__name__mangling_8h.html</anchorfile>
      <anchor>acc249283d938c05e57c318cc941089d4</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>psa_key_attributes_init</name>
      <anchorfile>psa__name__mangling_8h.html</anchorfile>
      <anchor>af1459851b1809faef849958f9d253d22</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>psa_get_key_attributes</name>
      <anchorfile>psa__name__mangling_8h.html</anchorfile>
      <anchor>a0437fe9a181dd2667b62f063f377ac0f</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>psa_reset_key_attributes</name>
      <anchorfile>psa__name__mangling_8h.html</anchorfile>
      <anchor>a74d46b380ac1e0c993b9fe14a7abef0e</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>psa_set_key_type</name>
      <anchorfile>psa__name__mangling_8h.html</anchorfile>
      <anchor>a0d97f52b7d6507cfb68dfc0705fbfeb6</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>psa_get_key_type</name>
      <anchorfile>psa__name__mangling_8h.html</anchorfile>
      <anchor>a6dc6769fc90f5e4c1b5be3e18e7db031</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>psa_set_key_bits</name>
      <anchorfile>psa__name__mangling_8h.html</anchorfile>
      <anchor>a99c0bf1dd7854db8365efa9d3c4e94a3</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>psa_get_key_bits</name>
      <anchorfile>psa__name__mangling_8h.html</anchorfile>
      <anchor>a24f385862aef135526d6063c96a35532</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>psa_set_key_lifetime</name>
      <anchorfile>psa__name__mangling_8h.html</anchorfile>
      <anchor>abe05b493d2160f06f01f9c1487db9402</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>psa_get_key_lifetime</name>
      <anchorfile>psa__name__mangling_8h.html</anchorfile>
      <anchor>aa1b6cbdf4a575acca64ea394f96fc25b</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>psa_set_key_id</name>
      <anchorfile>psa__name__mangling_8h.html</anchorfile>
      <anchor>aca2050f050a3b7edca597f2113cfb830</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>psa_get_key_id</name>
      <anchorfile>psa__name__mangling_8h.html</anchorfile>
      <anchor>aff8db4c667ae1035049750fc2be565c3</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>psa_set_key_algorithm</name>
      <anchorfile>psa__name__mangling_8h.html</anchorfile>
      <anchor>a49df3796739c5190afaa598f3bcd6ead</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>psa_get_key_algorithm</name>
      <anchorfile>psa__name__mangling_8h.html</anchorfile>
      <anchor>a89bb887ca2430cf169438c3df4f16875</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>psa_set_key_usage_flags</name>
      <anchorfile>psa__name__mangling_8h.html</anchorfile>
      <anchor>ad7582fd437683b213b94af3fdf0c38f0</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>psa_get_key_usage_flags</name>
      <anchorfile>psa__name__mangling_8h.html</anchorfile>
      <anchor>a1eb5358a47872b3c41eac097abf2e3a1</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>psa_import_key</name>
      <anchorfile>psa__name__mangling_8h.html</anchorfile>
      <anchor>a60d170807ab234052c01612005b77f98</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>psa_generate_key</name>
      <anchorfile>psa__name__mangling_8h.html</anchorfile>
      <anchor>a5100b4b72a0ae53887294df7bdbbe2f6</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>psa_copy_key</name>
      <anchorfile>psa__name__mangling_8h.html</anchorfile>
      <anchor>a92916bc98478a407d27a061a52c6eef5</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>psa_destroy_key</name>
      <anchorfile>psa__name__mangling_8h.html</anchorfile>
      <anchor>a4159111c5629867ea30c09b076efcf58</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>psa_purge_key</name>
      <anchorfile>psa__name__mangling_8h.html</anchorfile>
      <anchor>a2b48643c3889ddc197de876f4246e1e0</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>psa_export_key</name>
      <anchorfile>psa__name__mangling_8h.html</anchorfile>
      <anchor>a4923eb02ff4847a3a3336bc6c2362015</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>psa_export_public_key</name>
      <anchorfile>psa__name__mangling_8h.html</anchorfile>
      <anchor>a0baed840186cfe483b432e634e4f3a09</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>psa_hash_operation_init</name>
      <anchorfile>psa__name__mangling_8h.html</anchorfile>
      <anchor>acce17bac13b7fd86c5adc649611783af</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>psa_hash_setup</name>
      <anchorfile>psa__name__mangling_8h.html</anchorfile>
      <anchor>aa0f1f434be7b57bf24d9011bb5956089</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>psa_hash_update</name>
      <anchorfile>psa__name__mangling_8h.html</anchorfile>
      <anchor>a7e56a5acc07df193205bce7eb310050d</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>psa_hash_finish</name>
      <anchorfile>psa__name__mangling_8h.html</anchorfile>
      <anchor>a15b0d26fce0e9ad450031138080abf31</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>psa_hash_verify</name>
      <anchorfile>psa__name__mangling_8h.html</anchorfile>
      <anchor>aaa6192b7c3f18374499fe5f6466e440e</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>psa_hash_abort</name>
      <anchorfile>psa__name__mangling_8h.html</anchorfile>
      <anchor>a0ed69b3a0db43a897a192a4ea0a913df</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>psa_hash_clone</name>
      <anchorfile>psa__name__mangling_8h.html</anchorfile>
      <anchor>a906a3125d30e58bacb23e55198b20db8</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>psa_hash_compute</name>
      <anchorfile>psa__name__mangling_8h.html</anchorfile>
      <anchor>ae89727b45894128850eb25891fd831ee</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>psa_hash_compare</name>
      <anchorfile>psa__name__mangling_8h.html</anchorfile>
      <anchor>ad30cc3803ef830aced0194b7b7f1264c</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>psa_hash_suspend</name>
      <anchorfile>psa__name__mangling_8h.html</anchorfile>
      <anchor>afcf830be6fb20b9e1b3bc94ab77607fe</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>psa_hash_resume</name>
      <anchorfile>psa__name__mangling_8h.html</anchorfile>
      <anchor>ac31d14938a54beb886a45a2173aeafa6</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>psa_mac_operation_init</name>
      <anchorfile>psa__name__mangling_8h.html</anchorfile>
      <anchor>afd656b4929db333da8ef69abbaaa00d6</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>psa_mac_sign_setup</name>
      <anchorfile>psa__name__mangling_8h.html</anchorfile>
      <anchor>a909d6a79ef0a46f5d6250fb04d457466</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>psa_mac_verify_setup</name>
      <anchorfile>psa__name__mangling_8h.html</anchorfile>
      <anchor>acfb46cff6c4c0cf3902db96bc0cec5a1</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>psa_mac_update</name>
      <anchorfile>psa__name__mangling_8h.html</anchorfile>
      <anchor>abbd2b43e249b19f2fa84a28da925acc0</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>psa_mac_sign_finish</name>
      <anchorfile>psa__name__mangling_8h.html</anchorfile>
      <anchor>a56727df19d84a59628d01bc856aef3f6</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>psa_mac_verify_finish</name>
      <anchorfile>psa__name__mangling_8h.html</anchorfile>
      <anchor>a272e00da7cf6da4cad75d04019bf3ccd</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>psa_mac_compute</name>
      <anchorfile>psa__name__mangling_8h.html</anchorfile>
      <anchor>aac150dfa949b5e9539933c4421f7afe3</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>psa_mac_verify</name>
      <anchorfile>psa__name__mangling_8h.html</anchorfile>
      <anchor>ac82e96e9eedc1d17c2d0d5ddece465ef</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>psa_mac_abort</name>
      <anchorfile>psa__name__mangling_8h.html</anchorfile>
      <anchor>a16c98b062c0403d519c8667da8631da1</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>psa_cipher_encrypt</name>
      <anchorfile>psa__name__mangling_8h.html</anchorfile>
      <anchor>a291baf4db9aef280783f6081ca608447</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>psa_cipher_decrypt</name>
      <anchorfile>psa__name__mangling_8h.html</anchorfile>
      <anchor>a119cfc7f87542fdeba218223d3058ab5</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>psa_cipher_operation_init</name>
      <anchorfile>psa__name__mangling_8h.html</anchorfile>
      <anchor>a2af0c6cf5d97435194c23c3012102008</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>psa_cipher_encrypt_setup</name>
      <anchorfile>psa__name__mangling_8h.html</anchorfile>
      <anchor>a516d27d7e5b47bcefb6586ddacabb301</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>psa_cipher_decrypt_setup</name>
      <anchorfile>psa__name__mangling_8h.html</anchorfile>
      <anchor>a2e69680e667207664975db23b42a4984</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>psa_cipher_generate_iv</name>
      <anchorfile>psa__name__mangling_8h.html</anchorfile>
      <anchor>a3c53a694cac0e7239afe6cb622ab2157</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>psa_cipher_set_iv</name>
      <anchorfile>psa__name__mangling_8h.html</anchorfile>
      <anchor>a58df2268eab79e0ac29c345df8cbe71b</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>psa_cipher_update</name>
      <anchorfile>psa__name__mangling_8h.html</anchorfile>
      <anchor>a9e7b0a523b83efd6ba611d3db6a76372</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>psa_cipher_finish</name>
      <anchorfile>psa__name__mangling_8h.html</anchorfile>
      <anchor>a43d7abc90a57574cf603908f6243e7e6</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>psa_cipher_abort</name>
      <anchorfile>psa__name__mangling_8h.html</anchorfile>
      <anchor>a2abd9b71bf9b77a011330d2066865874</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>psa_aead_encrypt</name>
      <anchorfile>psa__name__mangling_8h.html</anchorfile>
      <anchor>a873c33300f40b545044fbc4ac448a952</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>psa_aead_decrypt</name>
      <anchorfile>psa__name__mangling_8h.html</anchorfile>
      <anchor>a80d237e39b2803b22ad57821ba9aa9d9</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>psa_aead_operation_init</name>
      <anchorfile>psa__name__mangling_8h.html</anchorfile>
      <anchor>aae24a00282111d64c7a99ff851859281</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>psa_aead_encrypt_setup</name>
      <anchorfile>psa__name__mangling_8h.html</anchorfile>
      <anchor>ab3e30830b14bbf6fdd16872b6b83c1d8</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>psa_aead_decrypt_setup</name>
      <anchorfile>psa__name__mangling_8h.html</anchorfile>
      <anchor>a9a43b25b12dea43001b4a4d92916f085</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>psa_aead_generate_nonce</name>
      <anchorfile>psa__name__mangling_8h.html</anchorfile>
      <anchor>a2d2b998e55a00f6abe1417e574a8dcb1</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>psa_aead_set_nonce</name>
      <anchorfile>psa__name__mangling_8h.html</anchorfile>
      <anchor>aa7ddb1980095c0074f1406100eda0e22</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>psa_aead_set_lengths</name>
      <anchorfile>psa__name__mangling_8h.html</anchorfile>
      <anchor>a997a9fdf99ced2ed01233158e834d729</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>psa_aead_update_ad</name>
      <anchorfile>psa__name__mangling_8h.html</anchorfile>
      <anchor>ac240ea8a75487730de512bab40fc504c</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>psa_aead_update</name>
      <anchorfile>psa__name__mangling_8h.html</anchorfile>
      <anchor>aa2974a9c13db2be1141cdc81b7e946b1</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>psa_aead_finish</name>
      <anchorfile>psa__name__mangling_8h.html</anchorfile>
      <anchor>a7334830b1871b32b6b71e74f674e0371</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>psa_aead_verify</name>
      <anchorfile>psa__name__mangling_8h.html</anchorfile>
      <anchor>af5d609c2a360ed13e870f181b51e96d2</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>psa_aead_abort</name>
      <anchorfile>psa__name__mangling_8h.html</anchorfile>
      <anchor>a15d3a5af0761aa621e7abd004c8f803a</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>psa_key_derivation_operation_init</name>
      <anchorfile>psa__name__mangling_8h.html</anchorfile>
      <anchor>a50c031c495c433f6502e679bb23d1c36</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>psa_key_derivation_setup</name>
      <anchorfile>psa__name__mangling_8h.html</anchorfile>
      <anchor>a060846ac906dd6e572cae41f4c4e31a3</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>psa_key_derivation_get_capacity</name>
      <anchorfile>psa__name__mangling_8h.html</anchorfile>
      <anchor>a0ed2767767caa530d0e48ea17b38e468</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>psa_key_derivation_set_capacity</name>
      <anchorfile>psa__name__mangling_8h.html</anchorfile>
      <anchor>aab05f54d1c35c4c65bd09ec8e68f9e46</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>psa_key_derivation_input_bytes</name>
      <anchorfile>psa__name__mangling_8h.html</anchorfile>
      <anchor>a970fe623d21acf9f045c4e2593f20f2e</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>psa_key_derivation_input_integer</name>
      <anchorfile>psa__name__mangling_8h.html</anchorfile>
      <anchor>ae8807d367bd70ad4a2e78027d9156ee6</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>psa_key_derivation_input_key</name>
      <anchorfile>psa__name__mangling_8h.html</anchorfile>
      <anchor>a1382705ac086a97719472e690243ba32</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>psa_key_derivation_output_bytes</name>
      <anchorfile>psa__name__mangling_8h.html</anchorfile>
      <anchor>ad8c018075c1f53a24210019895db48be</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>psa_key_derivation_output_key</name>
      <anchorfile>psa__name__mangling_8h.html</anchorfile>
      <anchor>a8ba0284182af53b034425528cdb7dc79</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>psa_key_derivation_verify_bytes</name>
      <anchorfile>psa__name__mangling_8h.html</anchorfile>
      <anchor>a67e8f0aa38040df7cfa5dcf19940beb7</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>psa_key_derivation_verify_key</name>
      <anchorfile>psa__name__mangling_8h.html</anchorfile>
      <anchor>a6ed9c3f80ef87431f6062f5559af3ab8</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>psa_key_derivation_abort</name>
      <anchorfile>psa__name__mangling_8h.html</anchorfile>
      <anchor>af41632ffbbeed3a021312b21657d9d47</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>psa_sign_message</name>
      <anchorfile>psa__name__mangling_8h.html</anchorfile>
      <anchor>a3e41af088e02e2fe6607b333a01e1ac6</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>psa_verify_message</name>
      <anchorfile>psa__name__mangling_8h.html</anchorfile>
      <anchor>a9c7abfaf4977c766057faac67e476a9f</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>psa_sign_hash</name>
      <anchorfile>psa__name__mangling_8h.html</anchorfile>
      <anchor>a6dc00225231ab59c0b96adc0c16017e1</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>psa_verify_hash</name>
      <anchorfile>psa__name__mangling_8h.html</anchorfile>
      <anchor>a9aac10cc29707d1f662b18e32c849252</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>psa_asymmetric_encrypt</name>
      <anchorfile>psa__name__mangling_8h.html</anchorfile>
      <anchor>a6ed38070e4f7442ab93f056e4863cd64</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>psa_asymmetric_decrypt</name>
      <anchorfile>psa__name__mangling_8h.html</anchorfile>
      <anchor>ac0a20970956d36fe8b9666f7c73c70ae</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>psa_key_derivation_key_agreement</name>
      <anchorfile>psa__name__mangling_8h.html</anchorfile>
      <anchor>a708ae105643e920cc33a3b43cea5ddbd</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>psa_raw_key_agreement</name>
      <anchorfile>psa__name__mangling_8h.html</anchorfile>
      <anchor>ab21bf63ae4cfb50ac2cad13113e98bef</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>psa_generate_random</name>
      <anchorfile>psa__name__mangling_8h.html</anchorfile>
      <anchor>a3c145c63feb142fe90c752caf0c5e889</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>psa_aead_operation_s</name>
    <filename>structpsa__aead__operation__s.html</filename>
    <member kind="variable">
      <type>psa_algorithm_t</type>
      <name>alg</name>
      <anchorfile>structpsa__aead__operation__s.html</anchorfile>
      <anchor>aed4a6c71316b99e2bcf4f63932850ef5</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>mxq_algo_id_t</type>
      <name>mxq_algo</name>
      <anchorfile>structpsa__aead__operation__s.html</anchorfile>
      <anchor>ae4857f28f8c1c3e4680699c2820a948b</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>psa_key_attributes_t</type>
      <name>key_attr</name>
      <anchorfile>structpsa__aead__operation__s.html</anchorfile>
      <anchor>aec6152597d4309dd3f66e7b8c0bd4c4d</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>ciph_params_t</type>
      <name>ciph_params</name>
      <anchorfile>structpsa__aead__operation__s.html</anchorfile>
      <anchor>a8acee0696b720a87895f8e5307a9903d</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uss_aead_ctx_t</type>
      <name>ctx</name>
      <anchorfile>structpsa__aead__operation__s.html</anchorfile>
      <anchor>aa429ca452fc4277618dbe08bf9e17c66</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>psa_cipher_operation_s</name>
    <filename>structpsa__cipher__operation__s.html</filename>
    <member kind="variable">
      <type>uint32_t</type>
      <name>handle</name>
      <anchorfile>structpsa__cipher__operation__s.html</anchorfile>
      <anchor>af8669d67e83042b64df48e13f87de934</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint32_t</type>
      <name>num_blocks_processed</name>
      <anchorfile>structpsa__cipher__operation__s.html</anchorfile>
      <anchor>a5f643e918966d59eee86adc253c74919</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>psa_key_attributes_t</type>
      <name>key_attr</name>
      <anchorfile>structpsa__cipher__operation__s.html</anchorfile>
      <anchor>a55ed41d3766b6e9087deace94a073d7d</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>mxq_algo_id_t</type>
      <name>cipher_algo</name>
      <anchorfile>structpsa__cipher__operation__s.html</anchorfile>
      <anchor>a99090a65212bca9ff397874a9e731cc8</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>ciph_params_t</type>
      <name>ciph_params</name>
      <anchorfile>structpsa__cipher__operation__s.html</anchorfile>
      <anchor>a3e974d6bd0eff665bb77a168f7d1a1ed</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uss_cipher_ctx_t</type>
      <name>ctx</name>
      <anchorfile>structpsa__cipher__operation__s.html</anchorfile>
      <anchor>a3242d9f0fd2bb8b522a944932eefd874</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>psa_key_attributes_t</name>
    <filename>structpsa__key__attributes__t.html</filename>
    <member kind="variable">
      <type>psa_key_type_t</type>
      <name>key_type</name>
      <anchorfile>structpsa__key__attributes__t.html</anchorfile>
      <anchor>a56029c3c6cca13a0bc9e832487f6cd87</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>psa_key_lifetime_t</type>
      <name>key_lifetime</name>
      <anchorfile>structpsa__key__attributes__t.html</anchorfile>
      <anchor>aa475982feb2245d538aea7a6c98b97cf</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>psa_key_usage_t</type>
      <name>key_use</name>
      <anchorfile>structpsa__key__attributes__t.html</anchorfile>
      <anchor>a44b36ab489b927537af2a4a8ac8f2b38</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>psa_algorithm_t</type>
      <name>key_algo</name>
      <anchorfile>structpsa__key__attributes__t.html</anchorfile>
      <anchor>aaddf72c3cdda282b7c25fd2cb21926ae</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>size_t</type>
      <name>key_bits</name>
      <anchorfile>structpsa__key__attributes__t.html</anchorfile>
      <anchor>a8319794b77e8bd79b2008b49ce9ec37f</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>psa_key_id_t</type>
      <name>key_id</name>
      <anchorfile>structpsa__key__attributes__t.html</anchorfile>
      <anchor>ab4b6786a4c3b5615b6a4c0b0d7767a67</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>psa_key_derivation_operation_s</name>
    <filename>structpsa__key__derivation__operation__s.html</filename>
    <member kind="variable">
      <type>uint32_t</type>
      <name>handle</name>
      <anchorfile>structpsa__key__derivation__operation__s.html</anchorfile>
      <anchor>a4a50e1098655de64043d42a3643d0adf</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>psa_mac_operation_s</name>
    <filename>structpsa__mac__operation__s.html</filename>
    <member kind="variable">
      <type>uint8_t</type>
      <name>perform</name>
      <anchorfile>structpsa__mac__operation__s.html</anchorfile>
      <anchor>aff637dc5f839fac6059c193bcf94b484</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint32_t</type>
      <name>key_id</name>
      <anchorfile>structpsa__mac__operation__s.html</anchorfile>
      <anchor>a93bb74d30d69e94d4106791634d7f156</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>psa_algorithm_t</type>
      <name>alg</name>
      <anchorfile>structpsa__mac__operation__s.html</anchorfile>
      <anchor>a241f3e4508cd19df2753ebde36144b07</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>mxq_algo_id_t</type>
      <name>mxq_algo</name>
      <anchorfile>structpsa__mac__operation__s.html</anchorfile>
      <anchor>a8773837dc0fab6cc4020b24b052712bf</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>size_t</type>
      <name>mac_length</name>
      <anchorfile>structpsa__mac__operation__s.html</anchorfile>
      <anchor>a88630a68300d74d7da8b5bd91eb57393</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>bool</type>
      <name>isActive</name>
      <anchorfile>structpsa__mac__operation__s.html</anchorfile>
      <anchor>a8461ccbcba56397185b40bb4cb6cec3b</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>psa_uss_hash_t</name>
    <filename>structpsa__uss__hash__t.html</filename>
    <member kind="variable">
      <type>psa_algorithm_t</type>
      <name>alg</name>
      <anchorfile>structpsa__uss__hash__t.html</anchorfile>
      <anchor>a62d924c8b04b7f6d9ae92eab41d94ceb</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>bool</type>
      <name>isInitialized</name>
      <anchorfile>structpsa__uss__hash__t.html</anchorfile>
      <anchor>a3c7ca000b07bebd6adf128c050c811a1</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>uss_aead_ctx_t</name>
    <filename>structuss__aead__ctx__t.html</filename>
    <member kind="variable">
      <type>uint8_t</type>
      <name>nonce</name>
      <anchorfile>structuss__aead__ctx__t.html</anchorfile>
      <anchor>a28744059091d6059386f6004ebf766e2</anchor>
      <arglist>[16]</arglist>
    </member>
    <member kind="variable">
      <type>uint32_t</type>
      <name>body_available</name>
      <anchorfile>structuss__aead__ctx__t.html</anchorfile>
      <anchor>a3ea9c5e5ba56701212102cb9bd154ce6</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint32_t</type>
      <name>aad_available</name>
      <anchorfile>structuss__aead__ctx__t.html</anchorfile>
      <anchor>ad15d80a0634428a4b258156849504984</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint32_t</type>
      <name>isActive</name>
      <anchorfile>structuss__aead__ctx__t.html</anchorfile>
      <anchor>a6880762566086b68869bcd9e27ea186e</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint32_t</type>
      <name>encrypt_decrypt</name>
      <anchorfile>structuss__aead__ctx__t.html</anchorfile>
      <anchor>a838cb4bd181e176b5559477e0939beb9</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint32_t</type>
      <name>isNonceSet</name>
      <anchorfile>structuss__aead__ctx__t.html</anchorfile>
      <anchor>a6a34cdb3b7f6a8c26d4a390da7dac13d</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint32_t</type>
      <name>isLenSet</name>
      <anchorfile>structuss__aead__ctx__t.html</anchorfile>
      <anchor>acc86e732582a569e202b086f1b8d9d7e</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint32_t</type>
      <name>isAADBegin</name>
      <anchorfile>structuss__aead__ctx__t.html</anchorfile>
      <anchor>a500f6199fb70cb8b0f51e6de33cb0ff7</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint32_t</type>
      <name>isBodyBegin</name>
      <anchorfile>structuss__aead__ctx__t.html</anchorfile>
      <anchor>a4d25fde8adabb93498e46fcc4f09c67a</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint32_t</type>
      <name>isCipherInitDone</name>
      <anchorfile>structuss__aead__ctx__t.html</anchorfile>
      <anchor>a7e79f53d441b824f351eac908f2fb86e</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>uss_cipher_ctx_t</name>
    <filename>structuss__cipher__ctx__t.html</filename>
    <member kind="variable">
      <type>uint8_t</type>
      <name>nonce</name>
      <anchorfile>structuss__cipher__ctx__t.html</anchorfile>
      <anchor>a220f68d5a3f60700d94b3bfd01287cc6</anchor>
      <arglist>[16]</arglist>
    </member>
    <member kind="variable">
      <type>uint32_t</type>
      <name>isActive</name>
      <anchorfile>structuss__cipher__ctx__t.html</anchorfile>
      <anchor>a00e5d5b8c141c06c3f398b75cb31ca7b</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint32_t</type>
      <name>encrypt_decrypt</name>
      <anchorfile>structuss__cipher__ctx__t.html</anchorfile>
      <anchor>a4a19c18c28a0f2816fcc070bb6645bcd</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint32_t</type>
      <name>isIVSet</name>
      <anchorfile>structuss__cipher__ctx__t.html</anchorfile>
      <anchor>a4d4cb4ed4e708c8b9f589748941cc8fe</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint32_t</type>
      <name>isBodyBegin</name>
      <anchorfile>structuss__cipher__ctx__t.html</anchorfile>
      <anchor>a312bf273a7fcef9af81d97c361f6cd5d</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint32_t</type>
      <name>isCipherInitDone</name>
      <anchorfile>structuss__cipher__ctx__t.html</anchorfile>
      <anchor>a541765f8d3e9a66c661b22f742b11ffd</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint32_t</type>
      <name>processed_input_len</name>
      <anchorfile>structuss__cipher__ctx__t.html</anchorfile>
      <anchor>a00cb05b1f410895af2b0c7602a49f1a5</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="page">
    <name>index</name>
    <title>USS Library PSA APIs</title>
    <filename>index.html</filename>
  </compound>
</tagfile>
